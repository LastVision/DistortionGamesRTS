#include "stdafx.h"

#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "BlendedBehavior.h"
#include "ControllerComponent.h"
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include <PathFinderAStar.h>
#include <PathFinderFunnel.h>
#include "PollingStation.h"
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
	, myAttackTarget(nullptr)
	, myActions(16)
	, myAttackTargetPathRefreshTime(0.5f)
	, myCurrentAttackTargetPathRefreshTime(myAttackTargetPathRefreshTime)
	, myBehavior(new BlendedBehavior(myEntity))
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

	myAttackTimer -= aDelta;

	if (myCurrentAction.myAction == eAction::IDLE)
	{
		myEntity.SetState(eEntityState::IDLE);
		if (myOwnerType == eOwnerType::NEUTRAL)
		{
			int apa = 5;
		}
		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, myTargetType, myVisionRange2);

		if (closeTarget != nullptr)
		{
			myAttackTarget = closeTarget;
			myReturnPosition = myEntity.myPosition;

			ActionData action;
			action.myAction = eAction::ATTACK_TARGET;
			action.myPosition = myAttackTarget->myPosition;
			myActions.Add(action);
		}
	}
	else if (myCurrentAction.myAction == eAction::ATTACK_MOVE)
	{
		if (myAttackTarget != nullptr)
		{
			DoAttackAction();
		}
		else
		{
			DoMoveAction(myCurrentAction.myPosition);

			myAttackTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
				, myTargetType, myVisionRange2 * 10.f);
		}
	}
	else if (myCurrentAction.myAction == eAction::RETURN)
	{
		DL_ASSERT_EXP(myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX
			, "ControllerComponent tried to do RETURN-Action without a valid ReturnPosition");

		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity({ myReturnPosition.x, 0, myReturnPosition.y }
			, myTargetType, myVisionRange2);

		if (closeTarget != nullptr)
		{
			myAttackTarget = closeTarget;
			myCurrentAction.myAction = eAction::ATTACK_TARGET;
		}
		else
		{
			if (myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX)
			{
				DoMoveAction(myReturnPosition);
			}
			else
			{
				myReturnPosition = myEntity.myPosition;
			}
		}
	}
	else if (myCurrentAction.myAction == eAction::HOLD_POSITION)
	{
		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, myTargetType, myAttackRange2);

		if (closeTarget != nullptr)
		{
			myAttackTarget = closeTarget;
			AttackTarget();
		}
	}
	else if (myCurrentAction.myAction == eAction::ATTACK_TARGET)
	{
		myCurrentAttackTargetPathRefreshTime -= aDelta;
		if (myCurrentAttackTargetPathRefreshTime <= 0.f)
		{
			RefreshPathToAttackTarget();
		}

		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, myTargetType, myAttackRange2);

		if (closeTarget != nullptr && closeTarget == myAttackTarget)
		{
			AttackTarget();
		}
		else
		{
			DoMoveAction(myCurrentAction.myPosition);
		}
	}
	else if (myCurrentAction.myAction == eAction::MOVE)
	{
		//DoMoveAction(myCurrentAction.myPosition);
	}


	if (myEntity.GetState() == eEntityState::IDLE)
	{
		StartNextAction();
	}

	myData.myAcceleration = myBehavior->Update();

	RenderDebugLines();
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	myEntity.SetState(eEntityState::IDLE);
	FillCommandList({ aPosition.x, aPosition.z }, eAction::MOVE, aClearCommandQueue);
}

void ControllerComponent::AttackTarget(Entity* aEntity, bool aClearCommandQueue)
{
	myAttackTarget = aEntity;
	FillCommandList(myAttackTarget->myPosition, eAction::ATTACK_TARGET, aClearCommandQueue);
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	FillCommandList({ aPosition.x, aPosition.z }, eAction::ATTACK_MOVE, aClearCommandQueue);

	Entity* target = nullptr;
	switch (myEntity.GetOwner())
	{
	case eOwnerType::PLAYER:
		target = PollingStation::GetInstance()->FindEntityAtPosition(aPosition, eOwnerType::ENEMY);
		break;
	case eOwnerType::ENEMY:
		target = PollingStation::GetInstance()->FindEntityAtPosition(aPosition, eOwnerType::PLAYER);
		break;
	default:
		DL_ASSERT("An Entity thats not owned by Player or AI tried to Attack");
		break;
	}

	myReturnPosition.x = FLT_MAX;
	myReturnPosition.y = FLT_MAX;

	if (myEntity.GetState() != eEntityState::ATTACKING || myAttackTarget != target)
	{
		myAttackTarget = target;
	}
}

void ControllerComponent::Stop()
{
	myActions.RemoveAll();
	myCurrentAction.myAction = eAction::IDLE;
	myCurrentAction.myPosition = myEntity.myPosition;
	myData.myAcceleration = { 0.f, 0.f };
	myAttackTarget = nullptr;
}

void ControllerComponent::HoldPosition()
{
	myActions.RemoveAll();
	myCurrentAction.myAction = eAction::HOLD_POSITION;
	myCurrentAction.myPosition = myEntity.myPosition;
	myData.myAcceleration = { 0.f, 0.f };
	myAttackTarget = nullptr;
	myEntity.SetState(eEntityState::IDLE);
}

void ControllerComponent::Spawn(const CU::Vector3f& aPosition)
{
	myEntity.myOrientation.SetPos(aPosition);
	myTerrain.CalcEntityHeight(myEntity.myOrientation);
}

void ControllerComponent::FillCommandList(const CU::Vector2<float>& aTargetPosition, eAction aAction, bool aClearCommandQueue)
{
	if (aClearCommandQueue == true)
	{
		myActions.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);
	}

	ActionData action;
	action.myAction = aAction;

	CU::GrowingArray<CU::Vector3<float>> path(16);
	if (myTerrain.GetPathFinder()->FindPath(myEntity.GetOrientation().GetPos(), { aTargetPosition.x, 0, aTargetPosition.y }, path) == true)
	{
		if (path.Size() > 0)
		{
			for (int i = 0; i < path.Size(); ++i)
			{
				CU::Vector2<float> target = { path[i].x, path[i].z };

				action.myPosition = target;

				myActions.Add(action);
			}
		}
		else
		{
			action.myPosition = aTargetPosition;
			myActions.Add(action);
		}
	}

	//if (aClearCommandQueue == true)
	//{
	//	StartNextAction();
	//}
}

void ControllerComponent::DoMoveAction(const CU::Vector2<float>& aTargetPosition)
{
	//if (myEntity.GetState() != eEntityState::WALKING) // derp?
	//{
	//	myEntity.SetState(eEntityState::WALKING);
	//}
	//else if (myEntity.GetState() == eEntityState::WALKING)
	//{
	//	//CU::Vector3<float> position = myEntity.myOrientation.GetPos();
	//	//myEntity.myOrientation.SetPos({ position.x, 0.f, position.z });

	//	//float dotResult = CU::Dot(myEntity.myOrientation.GetForward(), aTargetPosition - position);
	//	//if (dotResult <= 0)
	//	//{
	//	//	myEntity.myOrientation.SetPos(aTargetPosition);
	//	//	myData.myAcceleration = { 0.f, 0.f };
	//	//	myEntity.SetState(eEntityState::IDLE);
	//	//}

	//	//myEntity.myOrientation.SetPos(position);
	//}
}

void ControllerComponent::DoAttackAction()
{
	bool shouldReturn = false;
	bool hasReturnPosition = false;
	float distanceToTarget = CU::Length2(myEntity.GetOrientation().GetPos() - myAttackTarget->GetOrientation().GetPos());
	if (myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX)
	{
		float distanceToReturnPosition = CU::Length2(myEntity.myPosition - myReturnPosition);
		hasReturnPosition = true;
		float chaseDistance = myChaseDistance2;

		if (myEntity.GetOwner() == eOwnerType::NEUTRAL)
		{
			chaseDistance = myChaseDistanceNeutral2;
		}

		if (distanceToReturnPosition > chaseDistance)
		{
			shouldReturn = true;
		}
	}

	if (distanceToTarget < myAttackRange2 && shouldReturn == false)
	{
		AttackTarget();
		if (myAttackTarget->GetAlive() == false)
		{
			if (hasReturnPosition == true)
			{
				myCurrentAction.myAction = eAction::RETURN;
				myEntity.GetComponent<ActorComponent>()->LookAtPoint(myReturnPosition);
				myData.myAcceleration = myReturnPosition - myEntity.myPosition;					//should be in behavior
			}
			else
			{
				myAttackTarget = nullptr;
			}
		}
	}
	else if (shouldReturn == true)
	{
		myCurrentAction.myAction = eAction::RETURN;
		myEntity.GetComponent<ActorComponent>()->LookAtPoint(myReturnPosition);
		myData.myAcceleration = myReturnPosition - myEntity.myPosition;
	}
	else
	{
		DoMoveAction(myAttackTarget->myPosition);
	}
}

void ControllerComponent::AttackTarget()
{
	if (myEntity.GetState() == eEntityState::ATTACKING)
	{
		if (myAttackTimer <= 0.f)
		{
			myAttackTimer = myAttackRechargeTime;

			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				if (myEntity.GetComponent<GrenadeComponent>()->GetCooldown() <= 0.f)
				{
					myEntity.GetComponent<GrenadeComponent>()->ThrowGrenade(myAttackTarget->GetOrientation().GetPos());
				}
			}

			//bool targetSurvived = false;

			HealthComponent* targetHealth = myAttackTarget->GetComponent<HealthComponent>();
			if (targetHealth != nullptr && myAttackTarget->GetAlive())
			{
				targetHealth->TakeDamage(myAttackDamage, &myEntity);
			}

			//AnimationComponent* animation = myEntity.GetComponent<AnimationComponent>();
			//DL_ASSERT_EXP(animation != nullptr, "Animation missing from attacking unit");
			//if (animation != nullptr)
			//{
			//	animation->RestartCurrentAnimation();
			//}

			//if (targetSurvived == false)
			//{
			//	myAttackTarget->SetState(eEntityState::DYING);
			//	myEntity.SetState(eEntityState::IDLE);
			//}
		}
	}
	else
	{
		myEntity.SetState(eEntityState::ATTACKING);
		myData.myAcceleration = { 0.f, 0.f };
	}
}

void ControllerComponent::StartNextAction()
{
	if (myActions.Size() > 0)
	{
		myReturnPosition = myEntity.myPosition;
		myCurrentAction.myAction = myActions[0].myAction;
		myCurrentAction.myPosition = myActions[0].myPosition;
		myActions.RemoveNonCyclicAtIndex(0);
		myBehavior->SetTarget(myCurrentAction.myPosition);

		if (myCurrentAction.myAction == eAction::ATTACK_MOVE
			|| myCurrentAction.myAction == eAction::ATTACK_TARGET
			|| myCurrentAction.myAction == eAction::MOVE
			|| myCurrentAction.myAction == eAction::RETURN)
		{
			myEntity.SetState(eEntityState::WALKING);
		}
	}
	else
	{
		myCurrentAction.myAction = eAction::IDLE;
		myCurrentAction.myPosition = myEntity.myPosition;
	}
}

void ControllerComponent::RefreshPathToAttackTarget()
{
	AttackTarget(myAttackTarget, true);
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
			pointB = myTerrain.GetHeight(myCurrentAction.myPosition, 1.f);
		}

		Prism::RenderLine3D(pointA, pointB, GetActionColor(myCurrentAction.myAction));
	}

	if (myActions.Size() > 0)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(myCurrentAction.myPosition, 1.f);
		CU::Vector3<float> pointB = myTerrain.GetHeight(myActions[0].myPosition, 1.f);
		eColorDebug color = GetActionColor(myActions[0].myAction);

		Prism::RenderLine3D(pointA, pointB, color);

		for (int i = 1; i < myActions.Size(); ++i)
		{
			color = GetActionColor(myActions[i].myAction);

			if (myActions[i].myAction != eAction::IDLE)
			{
				pointA = myTerrain.GetHeight(myActions[i-1].myPosition, 1.f);
				pointB = myTerrain.GetHeight(myActions[i].myPosition, 1.f);
				Prism::RenderLine3D(pointA, pointB, color);
			}
		}
	}
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
	case ControllerComponent::eAction::HOLD_POSITION:
		return eColorDebug::YELLOW;
		break;
	default:
		DL_ASSERT("Tried to GetActionColor of a invalid action");
		break;
	}

	return eColorDebug::NOT_USED;
}
