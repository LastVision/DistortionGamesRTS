#include "stdafx.h"

#include <AudioInterface.h>
#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "BlendedBehavior.h"
#include "ControllerComponent.h"
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include <KillUnitMessage.h>
#include <PathFinderAStar.h>
#include <PathFinderFunnel.h>
#include "PollingStation.h"
#include "PromotionComponent.h"
#include <PostMaster.h>
#include "SoundComponent.h"
#include <Terrain.h>



ControllerComponent::ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, myVisionRange2(aData.myVisionRange * aData.myVisionRange)
	, myAttackRange2(aData.myAttackRange * aData.myAttackRange)
	, myAttackDamage(aData.myAttackDamage)
	, myAttackRechargeTime(aData.myAttackRechargeTime)
	, myAttackTimer(0.f)
	, myChaseDistance2(aData.myChaseDistance * aData.myChaseDistance)
	, myChaseDistanceNeutral2(aData.myChaseDistanceNeutral * aData.myChaseDistanceNeutral)
	, myCommands(16)
	, myAttackTargetPathRefreshTime(0.5f)
	, myCurrentAttackTargetPathRefreshTime(myAttackTargetPathRefreshTime)
	, myBehavior(new BlendedBehavior(myEntity))
	, myRangerOneShotTimer(0.f)
	, myRangerOneShotCooldown(aData.myRangerOneShotCoolDown)
{
	DL_ASSERT_EXP(myEntity.GetComponent<ActorComponent>() != nullptr
		, "ControllerComponent wont work without a ActorComponent");

	myOwnerType = myEntity.GetOwner();
	if (myOwnerType == eOwnerType::PLAYER)
	{
		myTargetType = eOwnerType::ENEMY | eOwnerType::NEUTRAL;
	}
	else if (myOwnerType == eOwnerType::ENEMY)
	{
		myTargetType = eOwnerType::PLAYER | eOwnerType::NEUTRAL;
	}
	else if (myOwnerType == eOwnerType::NEUTRAL)
	{
		myTargetType = eOwnerType::PLAYER | eOwnerType::ENEMY;
	}
	else
	{
		DL_ASSERT("An Entity not owned by Player, Enemy or Neutral tried to create a ControllerComponent");
	}
}


ControllerComponent::~ControllerComponent()
{
	SAFE_DELETE(myBehavior);
}

void ControllerComponent::Update(float aDelta)
{
	if (myEntity.GetState() == eEntityState::DIE)
	{
		return;
	}
	myRangerOneShotTimer -= aDelta;
	myAttackTimer -= aDelta;

	if (myBehavior->GetDone())
	{
		StartNextAction();
	}

	switch (myCurrentCommand.myCommand)
	{
	case eEntityCommand::STOP:
		DoStop();
		break;
	case eEntityCommand::MOVE:
		DoMove();
		break;
	case eEntityCommand::ATTACK_TARGET:
		DoAttackTarget(aDelta);
		break;
	case eEntityCommand::HOLD_POSITION:
		DoHold();
		break;
	case eEntityCommand::ATTACK_MOVE:
		DoAttackMove();
		break;
	default:
		DL_ASSERT("Unkown action");
		break;
	}

	myAcceleration = myBehavior->Update();

	RenderDebugLines();
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Move"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Move"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Move"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::AttackTarget(Entity* aEntity, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Attack"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Attack"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Attack"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::ATTACK_TARGET, aClearCommandQueue, aEntity);
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_AttackMove"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_AttackMove"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_AttackMove"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::ATTACK_MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::Stop(bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Stop"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Stop"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Stop"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	myCommands.RemoveAll();
	myCurrentCommand.myCommand = eEntityCommand::STOP;
	myCurrentCommand.myPosition = myEntity.myPosition;
	myAcceleration = { 0.f, 0.f };
	myEntity.SetCommand(myCurrentCommand.myCommand);
	CU::Vector2<float> newTargetPos = { myEntity.myPosition.x + myEntity.GetOrientation().GetForward().x,
		myEntity.myPosition.y + myEntity.GetOrientation().GetForward().z };
	myBehavior->SetTarget(newTargetPos);
}

void ControllerComponent::HoldPosition(bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Hold"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Hold"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Hold"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	myCommands.RemoveAll();
	myCurrentCommand.myCommand = eEntityCommand::HOLD_POSITION;
	myCurrentCommand.myPosition = myEntity.myPosition;
	myAcceleration = { 0.f, 0.f };
	myEntity.SetCommand(myCurrentCommand.myCommand);
	CU::Vector2<float> newTargetPos = { myEntity.myPosition.x + myEntity.GetOrientation().GetForward().x,
			myEntity.myPosition.y + myEntity.GetOrientation().GetForward().z };
	myBehavior->SetTarget(newTargetPos);
}

void ControllerComponent::FillCommandList(eEntityCommand aAction, bool aClearCommandQueue, Entity* aEntity
	, const CU::Vector2<float>& aTargetPosition)
{
	if (aClearCommandQueue == true)
	{
		myCommands.RemoveAll();
		//myEntity.SetState(eEntityState::IDLE);
		myEntity.SetCommand(eEntityCommand::STOP);
	}

	//find path within DoMove, or here later

	CommandData action;
	action.myCommand = aAction;
	action.myEntity = nullptr;
	//action.myPosition = aTargetPosition;
	//if (myCommands.Size() > 0)
	//{
	//	myCommands.Insert(0, action);
	//}
	//else
	//{
	//	myCommands.Add(action);
	//}

	CU::Vector3<float> targetPosition(aTargetPosition.x, 0, aTargetPosition.y);

	if (aEntity != nullptr)
	{
		targetPosition = aEntity->GetOrientation().GetPos();
	}

	CU::GrowingArray<CU::Vector3<float>> path(16);
	if (myTerrain.GetPathFinder()->FindPath(myEntity.GetOrientation().GetPos(), targetPosition, path) == true)
	{
		if (path.Size() > 0)
		{
			for (int i = 0; i < path.Size(); ++i)
			{
				CU::Vector2<float> target = { path[i].x, path[i].z };

				action.myPosition = target;

				if (myCommands.Size() > 0)
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myCommands.Insert(0, action);
				}
				else
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myCommands.Add(action);
				}
			}
		}
		else
		{
			action.myPosition = aTargetPosition;
			action.myEntity = aEntity;
			myCommands.Add(action);
		}
	}

	//if (aClearCommandQueue == true)
	//{
	//	StartNextAction();
	//}
}

void ControllerComponent::DoStop()
{
	myEntity.SetCommand(eEntityCommand::STOP);
	//myEntity.SetState(eEntityState::IDLE);

	Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
		, myTargetType, myVisionRange2);

	if (closestTarget != nullptr)
	{
		myReturnPosition = myEntity.myPosition;

		CommandData action;
		action.myCommand = eEntityCommand::ATTACK_TARGET;
		action.myEntity = closestTarget;
		myCommands.Add(action);
	}
}

void ControllerComponent::DoMove()
{
	myEntity.SetCommand(eEntityCommand::MOVE);
	myBehavior->SetTarget(GetPosition(myCurrentCommand));
}

void ControllerComponent::DoAttackTarget(float aDelta)
{
	if (myCurrentCommand.myEntity != nullptr && CU::Length2(myCurrentCommand.myEntity->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
	{
		AttackTarget();
	}
	else
	{
		myCurrentAttackTargetPathRefreshTime -= aDelta;
		if (myCurrentAttackTargetPathRefreshTime <= 0.f)
		{
			RefreshPathToAttackTarget();
		}

		DoMove();
	}
}

void ControllerComponent::DoHold()
{
	DL_ASSERT("Not implemented yet.");
}

void ControllerComponent::DoAttackMove()
{
	//myEntity.SetIntention(eEntityCommand::ATTACK_MOVE);
	myBehavior->SetTarget(GetPosition(myCurrentCommand));
	Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
		, myTargetType, myVisionRange2);

	if (closestTarget != nullptr)
	{
		myReturnPosition = myEntity.myPosition;

		CommandData action;
		action.myCommand = eEntityCommand::ATTACK_TARGET;
		action.myEntity = closestTarget;
		myCommands.Add(action);
	}
	else
	{
		DoMove();
	}
}

void ControllerComponent::AttackTarget()
{
	if (myAttackTimer <= 0.f)
	{
		//myEntity.SetState(eEntityState::ATTACK);
		myEntity.SetCommand(eEntityCommand::ATTACK_TARGET);
		myEntity.GetComponent<AnimationComponent>()->PlayAnimation(eEntityState::ATTACK);
		myAttackTimer = myAttackRechargeTime;

		if (myEntity.GetUnitType() == eUnitType::GRUNT)
		{
			if (myEntity.GetComponent<PromotionComponent>()->GetPromoted() == true
				&& myEntity.GetComponent<GrenadeComponent>()->GetCooldown() <= 0.f)
			{
				//myEntity.SetIntention(eEntityState::THROWING);
				myEntity.GetComponent<GrenadeComponent>()->ThrowGrenade(myCurrentCommand.myEntity->GetOrientation().GetPos());
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

		HealthComponent* targetHealth = myCurrentCommand.myEntity->GetComponent<HealthComponent>();
		bool targetSurvived = true; // if target is already dead, stay at "true" to not count as a kill
		if (targetHealth != nullptr && myCurrentCommand.myEntity->GetAlive())
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
			//myEntity.SetState(eEntityState::IDLE);
			myEntity.SetCommand(eEntityCommand::STOP);
			myBehavior->SetTarget(myEntity.GetPosition());
			PostMaster::GetInstance()->SendMessage(KillUnitMessage(static_cast<int>(myCurrentCommand.myEntity->GetUnitType()), 
				static_cast<int>(myCurrentCommand.myEntity->GetOwner())));
		}
	}
}

void ControllerComponent::StartNextAction()
{
	if (myCommands.Size() > 0)
	{
		myReturnPosition = myEntity.myPosition;
		myCurrentCommand.myCommand = myCommands.GetLast().myCommand;
		myCurrentCommand.myPosition = myCommands.GetLast().myPosition;
		myCurrentCommand.myEntity = myCommands.GetLast().myEntity;
		myCommands.RemoveCyclicAtIndex(myCommands.Size() - 1);
	}
	else
	{
		myCurrentCommand.myCommand = eEntityCommand::STOP;
		myCurrentCommand.myPosition = myEntity.myPosition;
		myCurrentCommand.myEntity = nullptr;
	}
}

void ControllerComponent::RefreshPathToAttackTarget()
{
	bool shouldBeQuiet = true;
	AttackTarget(myCurrentCommand.myEntity, true, shouldBeQuiet);
	myCurrentAttackTargetPathRefreshTime = myAttackTargetPathRefreshTime;
}

void ControllerComponent::RenderDebugLines() const
{
	if (myCurrentCommand.myCommand != eEntityCommand::STOP)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		CU::Vector3<float> pointB = myTerrain.GetHeight(GetPosition(myCurrentCommand), 1.f);

		Prism::RenderLine3D(pointA, pointB, GetActionColor(myCurrentCommand.myCommand));
	}

	if (myCommands.Size() > 0)
	{
		//CU::Vector3<float> pointA = myTerrain.GetHeight(GetPosition(myCurrentCommand), 1.f);
		CU::Vector3<float> pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		if (myCurrentCommand.myEntity != nullptr || myCurrentCommand.myPosition != CU::Vector2<float>(-1, -1))
		{
			CU::Vector3<float> pointA = myTerrain.GetHeight(GetPosition(myCurrentCommand), 1.f);
		}

		CU::Vector3<float> pointB = myTerrain.GetHeight(GetPosition(myCommands.GetLast()), 1.f);
		eColorDebug color = GetActionColor(myCommands.GetLast().myCommand);

		Prism::RenderLine3D(pointA, pointB, color);

		for (int i = myCommands.Size() - 1; i >= 1; --i)
		{
			color = GetActionColor(myCommands[i - 1].myCommand);


			pointA = myTerrain.GetHeight(GetPosition(myCommands[i]), 1.f);
			pointB = myTerrain.GetHeight(GetPosition(myCommands[i - 1]), 1.f);
			Prism::RenderLine3D(pointA, pointB, color);
		}
	}
}

const CU::Vector2<float>& ControllerComponent::GetPosition(const CommandData& anCommandData) const
{
	if (anCommandData.myEntity != nullptr)
	{
		return anCommandData.myEntity->GetPosition();
	}
	DL_ASSERT_EXP(anCommandData.myPosition.x != -1 && anCommandData.myPosition.y != -1, "Action Data not valid.");
	return anCommandData.myPosition;
}

eColorDebug ControllerComponent::GetActionColor(eEntityCommand aAction) const
{
	switch (aAction)
	{
	case eEntityCommand::STOP:
		return eColorDebug::WHITE;
		break;
	case eEntityCommand::MOVE:
		return eColorDebug::GREEN;
		break;
	case eEntityCommand::ATTACK_TARGET:
		return eColorDebug::RED;
		break;
	case eEntityCommand::ATTACK_MOVE:
		return eColorDebug::RED;
		break;
	case eEntityCommand::HOLD_POSITION:
		return eColorDebug::YELLOW;
		break;
	default:
		DL_ASSERT("Tried to GetActionColor of a invalid action");
		break;
	}

	return eColorDebug::NOT_USED;
}

void ControllerComponent::Reset()
{
	myTerrain.CalcEntityHeight(myEntity.myOrientation);
	myAcceleration = { 0.f, 0.f };
	myBehavior->SetTarget({ myEntity.GetPosition().x + CU::Math::RandomRange<float>(-0.1f, 0.1f), myEntity.GetPosition().y - 2.f });
	myCommands.RemoveAll();
}