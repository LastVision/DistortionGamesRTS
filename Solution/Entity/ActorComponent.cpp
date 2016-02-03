
#include "stdafx.h"

#include "ActorComponent.h"
#include <ArtifactMessage.h>
#include "AudioInterface.h"
#include "BehaviorNote.h"
#include "BlendedBehavior.h"
//#include "ActorComponentData.h"
#include "ControllerComponent.h"
#include <EmitterMessage.h>
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include "KillUnitMessage.h"
#include <Terrain.h>
#include "AnimationComponent.h"
#include "PollingStation.h"
#include <PostMaster.h>
#include "PromotionComponent.h"
#include "SoundComponent.h"

ActorComponent::ActorComponent(Entity& aEntity, ActorComponentData& aData, const Prism::Terrain& aTerrain
		, const EntityCommandData& aCurrentCommand)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, myVisionRange2(aData.myVisionRange * aData.myVisionRange)
	, myAttackRange2(aData.myAttackRange * aData.myAttackRange)
	, myAttackDamage(aData.myAttackDamage)
	, myAttackRechargeTime(aData.myAttackRechargeTime)
	, myAttackTimer(0.f)
	, myChaseDistance2(aData.myChaseDistance * aData.myChaseDistance)
	, myChaseDistanceNeutral2(aData.myChaseDistanceNeutral * aData.myChaseDistanceNeutral)
	, myAttackTargetPathRefreshTime(0.5f)
	, myCurrentAttackTargetPathRefreshTime(myAttackTargetPathRefreshTime)
	, myRangerOneShotTimer(0.f)
	, myRangerOneShotCooldown(aData.myRangerOneShotCoolDown)
	, myBehavior(new BlendedBehavior(myEntity))
	, myCurrentCommand(aCurrentCommand)
{
	myEntity.myMaxSpeed = aData.myMoveSpeed;

	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		myTargetType = eOwnerType::ENEMY | eOwnerType::NEUTRAL;
	}
	else if (myEntity.GetOwner() == eOwnerType::ENEMY)
	{
		myTargetType = eOwnerType::PLAYER | eOwnerType::NEUTRAL;
	}
	else if (myEntity.GetOwner() == eOwnerType::NEUTRAL)
	{
		myTargetType = eOwnerType::PLAYER | eOwnerType::ENEMY;
	}
	else
	{
		DL_ASSERT("An Entity not owned by Player, Enemy or Neutral tried to create a ControllerComponent");
	}
}

ActorComponent::~ActorComponent()
{
	SAFE_DELETE(myBehavior);
}

void ActorComponent::Reset()
{
	myAcceleration = CU::Vector2<float>(0.f, 0.f);
	myIsDone = false;
	myBehavior->SetTarget({ myEntity.GetPosition().x + CU::Math::RandomRange<float>(-0.1f, 0.1f), myEntity.GetPosition().y - 1.f });

}

void ActorComponent::Update(float aDelta)
{
	if (myEntity.GetAlive() == false || myEntity.GetState() == eEntityState::DIE)
	{
		return;
	}

	CheckAnimationComplete();

	myRangerOneShotTimer -= aDelta;
	myAttackTimer -= aDelta;

	if (myCurrentCommand != myPreviousCommand)
	{
		myBehavior->SetTarget(myCurrentCommand.GetPosition());
		myPreviousCommand = myCurrentCommand;
	}

	myAcceleration = myBehavior->Update(aDelta);

	if (myBehavior->GetDone() || myIsDone == true)
	{
		myEntity.SendNote<BehaviorNote>(true);
		myIsDone = false;
	}

	myEntity.myVelocity += myAcceleration * aDelta;
	float velocity2 = CU::Length2(myEntity.myVelocity);
	if (velocity2 > myEntity.myMaxSpeed * myEntity.myMaxSpeed)
	{
		CU::Normalize(myEntity.myVelocity);
		myEntity.myVelocity *= myEntity.myMaxSpeed;
	}


	switch (myCurrentCommand.myCommand)
	{
	case eEntityCommand::ATTACK_TARGET:
		DoAttackTarget(aDelta);
		break;
	case eEntityCommand::STOP:
		DoStop(aDelta);
		break;
	case eEntityCommand::MOVE:
		DoMove(aDelta);
		break;
	case eEntityCommand::ATTACK_MOVE:
		DoAttackMove(aDelta);
		break;
	case eEntityCommand::HOLD_POSITION:
		DoHoldPosition(aDelta);
		break;
	default:
		DL_ASSERT("Unknown intentionstate!");
		break;
	}

	if (myEntity.GetOwner() != eOwnerType::NEUTRAL)
	{
		const CU::GrowingArray<Entity*>& artifacts = PollingStation::GetInstance()->GetArtifacts();

		for (int i = artifacts.Size() - 1; i >= 0; --i)
		{
			if (CU::Length2(myEntity.GetPosition() - artifacts[i]->GetPosition()) < 5 * 5)
			{
				PostMaster::GetInstance()->SendMessage(ArtifactMessage(myEntity.GetOwner(), 1));
				PostMaster::GetInstance()->SendMessage(EmitterMessage("artifact_pickup", myEntity.GetOrientation().GetPos()));
				PollingStation::GetInstance()->DeleteArtifact(artifacts[i]);
			}
		}
	}
}

void ActorComponent::CheckAnimationComplete()
{
	if (myEntity.GetState() == eEntityState::ATTACK
		|| myEntity.GetState() == eEntityState::THROW)
	{
		if (myEntity.GetComponent<AnimationComponent>()->IsCurrentAnimationDone() == true)
		{
			myEntity.SetState(eEntityState::IDLE);
		}
	}
}

Entity* ActorComponent::FindAttackTarget()
{
	Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
		, myTargetType, myVisionRange2);

	if (closestTarget != nullptr
		&& CU::Length2(closestTarget->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
	{
		return closestTarget;
	}
	return nullptr;
}

void ActorComponent::DoStop(float aDelta)
{
	if (CU::Length2(myEntity.GetPosition() - myCurrentCommand.GetPosition()) > myChaseDistance2)
	{
		myBehavior->SetTarget(myCurrentCommand.GetPosition());
		DoMove(aDelta);
	}
	else
	{
		Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, myTargetType, myVisionRange2);

		if (closestTarget != nullptr)
		{
			if (CU::Length2(closestTarget->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
			{
				if (myAttackTimer < 0)
				{
					AttackTarget(closestTarget, aDelta);
				}
				else
				{
					StandStill();
				}
			}
			else
			{
				myBehavior->SetTarget(closestTarget->GetPosition());
				DoMove(aDelta);
			}
		}
		else
		{
			/*int nearbyFriends = PollingStation::GetInstance()->FindClosestEntities(myEntity.GetOrientation().GetPos(), 
				myEntity.GetOwner(), myVisionRange2).Size();*/
			if (myBehavior->GetDone() == false/* || CU::Length2(myEntity.GetPosition() - myCurrentCommand.GetPosition()) > 1.f * nearbyFriends*/)
			{
				myBehavior->SetTarget(myCurrentCommand.GetPosition());
				DoMove(aDelta);
			}
			else if (CU::Length(myAcceleration) > 1.f)
			{
				myBehavior->SetTarget(myEntity.GetPosition());
				DoMove(aDelta);
				myIsDone = true;
			}
			else
			{
				StandStill();
				myIsDone = true;
			}
		}
	}
}

void ActorComponent::DoAttackTarget(float aDelta)
{
	if (myCurrentCommand.myEntity != nullptr
		&& myCurrentCommand.myEntity->GetAlive() == true
		&& CU::Length2(myCurrentCommand.myEntity->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
	{
		if (myAttackTimer < 0)
		{
			AttackTarget(myCurrentCommand.myEntity, aDelta);
		}
		else
		{
			StandStill();
		}
	}
	else
	{
		DoMove(aDelta);
	}
}

void ActorComponent::DoAttackMove(float aDelta)
{
	Entity* closestTarget = FindAttackTarget();

	if (closestTarget != nullptr)
	{
		if (myAttackTimer < 0)
		{
			AttackTarget(closestTarget, aDelta);
		}
		else
		{
			StandStill();
		}
	}
	else
	{
		DoMove(aDelta);
	}
}


void ActorComponent::DoMove(float aDelta)
{
	LookInDirection(myEntity.myVelocity, aDelta);
	myEntity.SetState(eEntityState::WALK);
	CU::Vector2<float> position = myEntity.myPosition;

	position += myEntity.myVelocity * aDelta;
	myEntity.myOrientation.SetPos({ position.x, 0, position.y });
	myTerrain.CalcEntityHeight(myEntity.myOrientation);
}

void ActorComponent::DoHoldPosition(float aDelta)
{
	Entity* closestTarget = FindAttackTarget();

	if (closestTarget != nullptr)
	{
		if (myAttackTimer < 0)
		{
			AttackTarget(closestTarget, aDelta);
		}
		else
		{
			StandStill();
		}
	}
	else
	{
		StandStill();
	}
}

void ActorComponent::StandStill()
{
	//myAcceleration = { 0.f, 0.f };
	//myEntity.myVelocity = { 0.f, 0.f };


	if (myEntity.GetState() != eEntityState::ATTACK
		&& myEntity.GetState() != eEntityState::THROW)
	{
		myEntity.SetState(eEntityState::IDLE);
	}
}

void ActorComponent::AttackTarget(Entity* aTarget, float aDelta)
{
	if (aTarget->GetAlive() == false)
	{
		myEntity.SetState(eEntityState::IDLE);
		return;
	}
	if (myEntity.GetUnitType() == eUnitType::GRUNT && myEntity.GetState() == eEntityState::THROW &&
		myEntity.GetComponent<AnimationComponent>()->IsCurrentAnimationDone() == false)
	{
		return;
	}
	myEntity.SetState(eEntityState::ATTACK);
	LookInDirection(aTarget->GetPosition() - myEntity.GetPosition(), aDelta);
	
	myAttackTimer = myAttackRechargeTime;

	if (myEntity.GetUnitType() == eUnitType::GRUNT)
	{
		if (myEntity.GetComponent<PromotionComponent>()->GetPromoted() == true
			&& myEntity.GetComponent<GrenadeComponent>()->GetCooldown() <= 0.f)
		{
			myEntity.SetState(eEntityState::THROW);
			myEntity.GetComponent<GrenadeComponent>()->ThrowGrenade(aTarget->GetOrientation().GetPos());
		}
		else
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_MachineGun"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}
	}
	else if (myEntity.GetUnitType() == eUnitType::RANGER)
	{
		if (myRangerOneShotTimer <= 0.f)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_OneShot"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}
		else
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Sniper"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}

	}
	else if (myEntity.GetUnitType() == eUnitType::TANK)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_MiniGun"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
	}

	HealthComponent* targetHealth = aTarget->GetComponent<HealthComponent>();
	bool targetSurvived = true; // if target is already dead, stay at "true" to not count as a kill
	if (targetHealth != nullptr && aTarget->GetAlive())
	{
		if (myEntity.GetUnitType() == eUnitType::RANGER
			&& myEntity.GetComponent<PromotionComponent>()->GetPromoted() == true
			&& myRangerOneShotTimer <= 0.f)
		{
			myRangerOneShotTimer = myRangerOneShotCooldown;
			targetSurvived = targetHealth->TakeDamageAndCheckSurvive(targetHealth->GetMaxHealth() * 2.f);
		}
		else
		{
			targetSurvived = targetHealth->TakeDamageAndCheckSurvive(myAttackDamage);
		}
	}

	AnimationComponent* animation = myEntity.GetComponent<AnimationComponent>();
	DL_ASSERT_EXP(animation != nullptr, "Animation missing from attacking unit");
	if (animation != nullptr)
	{
		animation->RestartCurrentAnimation();
	}

	if (targetSurvived == false)
	{
		myEntity.GetComponent<PromotionComponent>()->EnemyKilled();
		myBehavior->SetTarget(myEntity.GetPosition());
		PostMaster::GetInstance()->SendMessage(KillUnitMessage(static_cast<int>(aTarget->GetUnitType()), 
			static_cast<int>(aTarget->GetOwner()), aTarget->GetComponent<ControllerComponent>()->GetTargetPosition()));
	}
}

void ActorComponent::LookInDirection(const CU::Vector2<float>& aDirection, float aDelta)
{
	CU::Vector2<float> direction2D = CU::GetNormalized(aDirection);
	CU::Vector3<float> direction = { direction2D.x, 0, direction2D.y };

	CU::Vector3<float> forward = myEntity.myOrientation.GetForward();
	CU::Normalize(forward);

	if (forward != direction)
	{
		CU::Vector3<float> planeNormal(0, 1.f, 0);
		float dot = CU::Dot(forward, direction);
		float det = CU::Dot(planeNormal, CU::Cross(forward, direction));

		float angle = atan2(det, dot);
		float limit = (M_PI) * aDelta;
		if (angle > limit)
		{
			angle = limit;
		}
		else if (angle < -limit)
		{
			angle = -limit;
		}


		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ 0.f, 0.f, 0.f });
		myEntity.myOrientation = myEntity.myOrientation * CU::Matrix44<float>::CreateRotateAroundY(angle);
		myEntity.myOrientation.SetPos(pos);
	}
}

void ActorComponent::LookAtPoint(const CU::Vector2<float>& aPoint, float aDelta)
{
	CU::Vector2<float> direction = aPoint - myEntity.myPosition;
	LookInDirection(direction, aDelta);
}