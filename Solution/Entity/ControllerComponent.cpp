#include "stdafx.h"

#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "ControllerComponent.h"
#include "ControllerComponentData.h"
#include "HealthComponent.h"
#include <PathFinder.h>
#include "PollingStation.h"
#include <Terrain.h>

ControllerComponent::ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, myVisionRange(aData.myVisionRange * aData.myVisionRange)
	, myAttackRange(aData.myAttackRange * aData.myAttackRange)
	, myAttackTarget(nullptr)
	, myActions(16)
	, myAttackTargetPathRefreshTime(0.5f)
	, myCurrentAttackTargetPathRefreshTime(myAttackTargetPathRefreshTime)
{
	DL_ASSERT_EXP(myEntity.GetComponent<ActorComponent>() != nullptr
		, "ControllerComponent wont work without a ActorComponent");

	myOwnerType = myEntity.GetOwner();
	if (myOwnerType == eOwnerType::PLAYER)
	{
		myTargetType = eOwnerType::ENEMY;
	}
	else if (myOwnerType == eOwnerType::ENEMY)
	{
		myTargetType = eOwnerType::PLAYER;
	}
	else
	{
		DL_ASSERT("An Entity not owned by Player or Enemy tried to create a ControllerComponent");
	}
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Update(float aDelta)
{
	if (myCurrentAction.myAction == eAction::IDLE)
	{
		myEntity.SetState(eEntityState::IDLE);

		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, myTargetType, myVisionRange);

		if (closeTarget != nullptr)
		{
			myAttackTarget = closeTarget;
			myReturnPosition = myEntity.GetOrientation().GetPos();
			myReturnPosition.y = 0.f;

			ActionData action;
			action.myAction = eAction::ATTACK_TARGET;
			action.myPosition = myAttackTarget->GetOrientation().GetPos();
			action.myPosition.y = 0.f;
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
				, myTargetType, myVisionRange);
		}
	}
	else if (myCurrentAction.myAction == eAction::RETURN)
	{
		DL_ASSERT_EXP(myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX
			, "ControllerComponent tried to do RETURN-Action without a valid ReturnPosition");

		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myReturnPosition
			, myTargetType, myVisionRange);

		if (closeTarget != nullptr)
		{
			myAttackTarget = closeTarget;
			myCurrentAction.myAction = eAction::ATTACK_TARGET;
		}
		else
		{
			if (myReturnPosition.x != FLT_MAX && myReturnPosition.z != FLT_MAX)
			{
				DoMoveAction(myReturnPosition);
			}
			else
			{
				myReturnPosition = myEntity.myOrientation.GetPos();
				myReturnPosition.y = 0.f;
			}
		}
	}
	else if (myCurrentAction.myAction == eAction::HOLD_POSITION)
	{
		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, myTargetType, myAttackRange);

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
			, myTargetType, myAttackRange);

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
		DoMoveAction(myCurrentAction.myPosition);
	}


	if (myEntity.GetState() == eEntityState::IDLE)
	{
		StartNextAction();
	}


	RenderDebugLines();
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	FillCommandList(aPosition, eAction::MOVE, aClearCommandQueue);
}

void ControllerComponent::AttackTarget(Entity* aEntity, bool aClearCommandQueue)
{
	myAttackTarget = aEntity;
	FillCommandList(myAttackTarget->GetOrientation().GetPos(), eAction::ATTACK_TARGET, aClearCommandQueue);
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	FillCommandList(aPosition, eAction::ATTACK_MOVE, aClearCommandQueue);

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
	myCurrentAction.myPosition = myEntity.GetOrientation().GetPos();
	myData.myDirection = { 0.f, 0.f, 0.f };
	myAttackTarget = nullptr;
}

void ControllerComponent::HoldPosition()
{
	myActions.RemoveAll();
	myCurrentAction.myAction = eAction::HOLD_POSITION;
	myCurrentAction.myPosition = myEntity.GetOrientation().GetPos();
	myData.myDirection = { 0.f, 0.f, 0.f };
	myAttackTarget = nullptr;
	myEntity.SetState(eEntityState::IDLE);
}

void ControllerComponent::Spawn(const CU::Vector3f& aPosition)
{
	myEntity.myOrientation.SetPos(aPosition);
	myTerrain.CalcEntityHeight(myEntity.myOrientation);
}

void ControllerComponent::FillCommandList(const CU::Vector3<float>& aTargetPosition, eAction aAction, bool aClearCommandQueue)
{
	if (aClearCommandQueue == true)
	{
		myActions.RemoveAll();
	}

	ActionData action;
	action.myAction = aAction;

	CU::GrowingArray<Prism::Navigation::Triangle*> path(16);
	if (myTerrain.GetPathFinder()->FindPath(myEntity.GetOrientation().GetPos(), aTargetPosition, path) == true)
	{
		for (int i = path.Size() - 1; i >= 0; --i)
		{
			CU::Vector3<float> target = path[i]->GetCenter();
			target.y = 0.f;

			action.myPosition = target;

			myActions.Add(action);
		}
	}

	if (aClearCommandQueue == true)
	{
		StartNextAction();
	}
}

void ControllerComponent::DoMoveAction(const CU::Vector3<float>& aTargetPosition)
{
	if (myEntity.GetState() != eEntityState::WALKING)
	{
		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		pos.y = 0.f;
		myData.myDirection = aTargetPosition - pos;
		myEntity.SetState(eEntityState::WALKING);
	}
	else if (myEntity.GetState() == eEntityState::WALKING)
	{
		CU::Vector3<float> position = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ position.x, 0.f, position.z });

		float dotResult = CU::Dot(myEntity.myOrientation.GetForward(), aTargetPosition - position);
		if (dotResult <= 0)
		{
			myEntity.myOrientation.SetPos(aTargetPosition);
			myData.myDirection = { 0.f, 0.f, 0.f };
			myEntity.SetState(eEntityState::IDLE);
		}

		myEntity.myOrientation.SetPos(position);
	}
}

void ControllerComponent::DoAttackAction()
{
	bool shouldReturn = false;
	bool hasReturnPosition = false;
	float distanceToTarget = CU::Length2(myEntity.GetOrientation().GetPos() - myAttackTarget->GetOrientation().GetPos());
	if (myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX)
	{
		float distanceToReturnPosition = CU::Length2(myEntity.GetOrientation().GetPos() - myReturnPosition);
		hasReturnPosition = true;
		if (distanceToReturnPosition > 100 /*maxChaseDistance*/)
		{
			shouldReturn = true;
		}
	}

	if (distanceToTarget < myAttackRange && shouldReturn == false)
	{
		AttackTarget();
		if (myAttackTarget->GetAlive() == false)
		{
			if (hasReturnPosition == true)
			{
				myCurrentAction.myAction = eAction::RETURN;
				myEntity.GetComponent<ActorComponent>()->LookAtPoint(myReturnPosition);
				CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
				pos.y = 0.f;
				myData.myDirection = myReturnPosition - pos;
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
		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		pos.y = 0.f;
		myData.myDirection = myReturnPosition - pos;
	}
	else
	{
		CU::Vector3<float> pos = myAttackTarget->GetOrientation().GetPos();
		pos.y = 0.f;
		DoMoveAction(pos);
	}
}

void ControllerComponent::AttackTarget()
{
	if (myEntity.GetState() == eEntityState::ATTACKING)
	{
		AnimationComponent* animation = myEntity.GetComponent<AnimationComponent>();
		if (animation != nullptr && animation->IsCurrentAnimationDone())
		{
			bool targetSurvived = false;

			HealthComponent* targetHealth = myAttackTarget->GetComponent<HealthComponent>();
			if (targetHealth != nullptr && myAttackTarget->GetAlive())
			{
				targetSurvived = targetHealth->TakeDamage(5);
			}

			if (targetSurvived == true)
			{
				animation->RestartCurrentAnimation();
			}
			else
			{
				myEntity.SetState(eEntityState::IDLE);
			}
		}
	}
	else
	{
		myEntity.SetState(eEntityState::ATTACKING);
		myData.myDirection = { 0.f, 0.f, 0.f };
	}
}

void ControllerComponent::StartNextAction()
{
	if (myActions.Size() > 0)
	{
		myReturnPosition = myEntity.GetOrientation().GetPos();
		myCurrentAction.myAction = myActions[0].myAction;
		myCurrentAction.myPosition = myActions[0].myPosition;
		myActions.RemoveNonCyclicAtIndex(0);
	}
	else
	{
		myCurrentAction.myAction = eAction::IDLE;
		myCurrentAction.myPosition = myEntity.GetOrientation().GetPos();
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
