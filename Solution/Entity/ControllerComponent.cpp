#include "stdafx.h"

#include <AudioInterface.h>
#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "BlendedBehavior.h"
#include "ControllerComponent.h"
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include <PathFinderAStar.h>
#include <PathFinderFunnel.h>
#include "PollingStation.h"
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
	, myActions(16)
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
	if (myEntity.GetState() == eEntityState::DYING)
	{
		return;
	}
	myRangerOneShotTimer -= aDelta;
	myAttackTimer -= aDelta;

	if (myEntity.GetState() == eEntityState::IDLE || myBehavior->GetDone())
	{
		StartNextAction();
	}

	switch (myCurrentAction.myAction)
	{
	case eAction::IDLE:
		DoIdle();
		break;
	case eAction::MOVE:
		DoMove();
		break;
	case eAction::ATTACK_TARGET:
		DoAttackTarget(aDelta);
		break;
	case eAction::RETURN:
		DoReturn();
		break;
	case eAction::HOLD:
		DoHold();
		break;
	case eAction::ATTACK_MOVE:
		DoAttackMove();
		break;
	default:
		DL_ASSERT("Unkown action");
		break;
	}

	myAcceleration = myBehavior->Update();

	RenderDebugLines();
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
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
	}
	FillCommandList(eAction::MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::AttackTarget(Entity* aEntity, bool aClearCommandQueue)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
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
	}
	FillCommandList(eAction::ATTACK_TARGET, aClearCommandQueue, aEntity);
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
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
	}
	FillCommandList(eAction::ATTACK_MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::Stop()
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
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
	}
	myActions.RemoveAll();
	myCurrentAction.myAction = eAction::IDLE;
	myCurrentAction.myPosition = myEntity.myPosition;
	myAcceleration = { 0.f, 0.f };
}

void ControllerComponent::HoldPosition()
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
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
	}
	myActions.RemoveAll();
	myCurrentAction.myAction = eAction::HOLD;
	myCurrentAction.myPosition = myEntity.myPosition;
	myAcceleration = { 0.f, 0.f };
	myEntity.SetState(eEntityState::IDLE);
}

void ControllerComponent::FillCommandList(eAction aAction, bool aClearCommandQueue, Entity* aEntity
	, const CU::Vector2<float>& aTargetPosition)
{
	if (aClearCommandQueue == true)
	{
		myActions.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);
	}

	//find path within DoMove, or here later

	ActionData action;
	action.myAction = aAction;
	action.myEntity = nullptr;
	//action.myPosition = aTargetPosition;
	//if (myActions.Size() > 0)
	//{
	//	myActions.Insert(0, action);
	//}
	//else
	//{
	//	myActions.Add(action);
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

				if (myActions.Size() > 0)
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myActions.Insert(0, action);
				}
				else
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myActions.Add(action);
				}
			}
		}
		else
		{
			action.myPosition = aTargetPosition;
			action.myEntity = aEntity;
			myActions.Add(action);
		}
	}

	//if (aClearCommandQueue == true)
	//{
	//	StartNextAction();
	//}
}

void ControllerComponent::DoIdle()
{
	myEntity.SetState(eEntityState::IDLE);

	Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
		, myTargetType, myVisionRange2);

	if (closestTarget != nullptr)
	{
		myReturnPosition = myEntity.myPosition;

		ActionData action;
		action.myAction = eAction::ATTACK_TARGET;
		action.myEntity = closestTarget;
		myActions.Add(action);
	}
}

void ControllerComponent::DoMove()
{
	myBehavior->SetTarget(GetPosition(myCurrentAction));
}

void ControllerComponent::DoAttackTarget(float aDelta)
{
	myCurrentAttackTargetPathRefreshTime -= aDelta;
	if (myCurrentAttackTargetPathRefreshTime <= 0.f)
	{
		RefreshPathToAttackTarget();
	}


	if (myCurrentAction.myEntity != nullptr && CU::Length2(myCurrentAction.myEntity->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
	{
		AttackTarget();
	}
	else
	{
		DoMove();
	}
}

void ControllerComponent::DoReturn()
{
	DL_ASSERT("Not implemented yet.");
}

void ControllerComponent::DoHold()
{
	DL_ASSERT("Not implemented yet.");
}

void ControllerComponent::DoAttackMove()
{
	Entity* closestTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
		, myTargetType, myVisionRange2);

	if (closestTarget != nullptr)
	{
		myReturnPosition = myEntity.myPosition;

		ActionData action;
		action.myAction = eAction::ATTACK_TARGET;
		action.myEntity = closestTarget;
		myActions.Add(action);
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
		myEntity.SetState(eEntityState::ATTACKING);
		myAttackTimer = myAttackRechargeTime;

		if (myEntity.GetUnitType() == eUnitType::GRUNT)
		{
			if (myEntity.GetComponent<GrenadeComponent>()->GetCooldown() <= 0.f)
			{
				myEntity.GetComponent<GrenadeComponent>()->ThrowGrenade(myCurrentAction.myEntity->GetOrientation().GetPos());
			}
			else
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_MachineGun"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
		else if (myEntity.GetUnitType() == eUnitType::RANGER)
		{
			if (myRangerOneShotCooldown <= 0.f)
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

		HealthComponent* targetHealth = myCurrentAction.myEntity->GetComponent<HealthComponent>();
		bool targetSurvived = false;
		if (targetHealth != nullptr && myCurrentAction.myEntity->GetAlive())
		{
			if (myEntity.GetUnitType() == eUnitType::RANGER && myRangerOneShotTimer <= 0.f)
			{
				myRangerOneShotTimer = myRangerOneShotCooldown;
				targetSurvived = targetHealth->TakeDamage(targetHealth->GetMaxHealth() * 2.f);
			}
			else
			{
				targetSurvived = targetHealth->TakeDamage(myAttackDamage);
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
			myEntity.SetState(eEntityState::IDLE);
			myBehavior->SetTarget(myEntity.GetPosition());
		}
	}
}

void ControllerComponent::StartNextAction()
{
	if (myActions.Size() > 0)
	{
		myReturnPosition = myEntity.myPosition;
		myCurrentAction.myAction = myActions.GetLast().myAction;
		myCurrentAction.myPosition = myActions.GetLast().myPosition;
		myCurrentAction.myEntity = myActions.GetLast().myEntity;
		myActions.RemoveCyclicAtIndex(myActions.Size() - 1);
	}
	else
	{
		myCurrentAction.myAction = eAction::IDLE;
		myCurrentAction.myPosition = myEntity.myPosition;
		myCurrentAction.myEntity = nullptr;
	}
}

void ControllerComponent::RefreshPathToAttackTarget()
{
	AttackTarget(myCurrentAction.myEntity, true);
	myCurrentAttackTargetPathRefreshTime = myAttackTargetPathRefreshTime;
}

void ControllerComponent::RenderDebugLines() const
{
	if (myCurrentAction.myAction != eAction::IDLE)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		CU::Vector3<float> pointB;
		if (myCurrentAction.myAction == eAction::RETURN)
		{
			pointB = myTerrain.GetHeight(myReturnPosition, 1.f);
		}
		else
		{
			pointB = myTerrain.GetHeight(GetPosition(myCurrentAction), 1.f);
		}

		Prism::RenderLine3D(pointA, pointB, GetActionColor(myCurrentAction.myAction));
	}

	if (myActions.Size() > 0)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(GetPosition(myCurrentAction), 1.f);

		CU::Vector3<float> pointB = myTerrain.GetHeight(GetPosition(myActions.GetLast()), 1.f);
		eColorDebug color = GetActionColor(myActions.GetLast().myAction);

		Prism::RenderLine3D(pointA, pointB, color);

		for (int i = myActions.Size() - 1; i >= 1; --i)
		{
			color = GetActionColor(myActions[i - 1].myAction);


			pointA = myTerrain.GetHeight(GetPosition(myActions[i]), 1.f);
			pointB = myTerrain.GetHeight(GetPosition(myActions[i - 1]), 1.f);
			Prism::RenderLine3D(pointA, pointB, color);
		}
	}
}

const CU::Vector2<float>& ControllerComponent::GetPosition(const ActionData& anActionData) const
{
	if (anActionData.myEntity != nullptr)
	{
		return anActionData.myEntity->GetPosition();
	}
	DL_ASSERT_EXP(anActionData.myPosition.x != -1 && anActionData.myPosition.y != -1, "Action Data not valid.");
	return anActionData.myPosition;
}

eColorDebug ControllerComponent::GetActionColor(eAction aAction) const
{
	switch (aAction)
	{
	case ControllerComponent::eAction::IDLE:
		return eColorDebug::WHITE;
		break;
	case ControllerComponent::eAction::MOVE:
		return eColorDebug::GREEN;
		break;
	case ControllerComponent::eAction::ATTACK_TARGET:
		return eColorDebug::RED;
		break;
	case ControllerComponent::eAction::ATTACK_MOVE:
		return eColorDebug::RED;
		break;
	case ControllerComponent::eAction::RETURN:
		return eColorDebug::YELLOW;
		break;
	case ControllerComponent::eAction::HOLD:
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
}