#include "stdafx.h"

#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "ControllerComponent.h"
#include "ControllerComponentData.h"
#include "HealthComponent.h"
#include "Entity.h"
#include "PollingStation.h"
#include <Terrain.h>

ControllerComponent::ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, myWayPoints(32)
	, myVisionRange(aData.myVisionRange * aData.myVisionRange)
	, myAttackRange(aData.myAttackRange * aData.myAttackRange)
	, myAttackTarget(nullptr)
	, myActions(16)
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
	if (myCurrentAction == eAction::IDLE)
	{
		myEntity.SetState(eEntityState::IDLE);

		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, myTargetType, myVisionRange);

		if (closeTarget != nullptr)
		{
			myAttackTarget = closeTarget;
			myReturnPosition = myEntity.GetOrientation().GetPos();


			ActionData action;
			action.myAction = eAction::ATTACK;
			action.myPosition = myAttackTarget->GetOrientation().GetPos();
			myActions.Add(action);
		}
	}
	else if (myCurrentAction == eAction::ATTACK)
	{
		if (myAttackTarget != nullptr)
		{
			DoAttackAction();
		}
		else
		{
			DoMoveAction(myCurrentActionPosition);

			myAttackTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
				, myTargetType, myVisionRange);
		}
	}
	else if (myCurrentAction == eAction::RETURN)
	{
		DL_ASSERT_EXP(myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX
			, "ControllerComponent tried to do RETURN-Action without a valid ReturnPosition");

		Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myReturnPosition
			, myTargetType, myVisionRange);

		if (closeTarget != nullptr)
		{
			myAttackTarget = closeTarget;
			myCurrentAction = eAction::ATTACK;
		}
		else
		{
			if (myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX)
			{
				DoMoveAction(myReturnPosition);
			}
			else
			{
				myReturnPosition = myEntity.myOrientation.GetPos();
			}
		}
	}
	else if (myCurrentAction == eAction::MOVE)
	{
		DoMoveAction(myCurrentActionPosition);
	}


	if (myEntity.GetState() == eEntityState::IDLE)
	{
		StartNextAction();
	}


	RenderDebugLines();
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	if (aClearCommandQueue == true)
	{
		myActions.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);
	}

	ActionData action;
	action.myPosition = aPosition;
	action.myAction = eAction::MOVE;

	myActions.Add(action);

	if (aClearCommandQueue == true)
	{
		StartNextAction();
	}
}

void ControllerComponent::Attack(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	if (aClearCommandQueue == true)
	{
		myActions.RemoveAll();
	}

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

	ActionData action;
	action.myAction = eAction::ATTACK;
	action.myPosition = aPosition;

	myReturnPosition.x = FLT_MAX;
	myReturnPosition.y = FLT_MAX;

	if (myEntity.GetState() != eEntityState::ATTACKING || myAttackTarget != target)
	{
		myAttackTarget = target;
	}

	myActions.Add(action);

	if (aClearCommandQueue == true)
	{
		StartNextAction();
	}
}

void ControllerComponent::Stop()
{
	myActions.RemoveAll();
	myCurrentAction = eAction::IDLE;
	myCurrentActionPosition = myEntity.GetOrientation().GetPos();
	myData.myDirection = { 0.f, 0.f, 0.f };
	myAttackTarget = nullptr;
}

void ControllerComponent::Spawn(const CU::Vector3f& aPosition)
{
	myEntity.myOrientation.SetPos(aPosition);
	myTerrain.CalcEntityHeight(myEntity.myOrientation);
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
				myCurrentAction = eAction::RETURN;
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
		myCurrentAction = eAction::RETURN;
		myEntity.GetComponent<ActorComponent>()->LookAtPoint(myReturnPosition);
		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		pos.y = 0.f;
		myData.myDirection = myReturnPosition - pos;
	}
	else
	{
		DoMoveAction(myAttackTarget->GetOrientation().GetPos());
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
			if (targetHealth != nullptr)
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
		myCurrentAction = myActions[0].myAction;
		myCurrentActionPosition = myActions[0].myPosition;
		myActions.RemoveNonCyclicAtIndex(0);
	}
	else
	{
		myCurrentAction = eAction::IDLE;
		myCurrentActionPosition = myEntity.GetOrientation().GetPos();
	}
}

void ControllerComponent::RenderDebugLines() const
{
	if (myEntity.GetState() == eEntityState::WALKING)
	{
		CU::Vector3<float> targetPosition = myTerrain.GetHeight(myMoveTarget, 2.f);
		Prism::RenderBox(targetPosition, eColorDebug::BLUE);
		Prism::RenderLine3D(myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 2.f), targetPosition);

		if (myWayPoints.Size() > 0)
		{
			Prism::RenderBox(myTerrain.GetHeight(myWayPoints[0], 2.f), eColorDebug::GREEN);
			Prism::RenderLine3D(targetPosition, myTerrain.GetHeight(myWayPoints[0], 2.f));

			for (int i = 1; i < myWayPoints.Size(); ++i)
			{
				Prism::RenderLine3D(myTerrain.GetHeight(myWayPoints[i - 1], 2.f), myTerrain.GetHeight(myWayPoints[i], 2.f));
				Prism::RenderBox(myTerrain.GetHeight(myWayPoints[i], 2.f), eColorDebug::GREEN);
			}
		}
	}


	if (myCurrentAction != eAction::IDLE)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		CU::Vector3<float> pointB;
		if (myCurrentAction == eAction::RETURN)
		{
			pointB = myTerrain.GetHeight(myReturnPosition, 1.f);
		}
		else
		{
			pointB = myTerrain.GetHeight(myCurrentActionPosition, 1.f);
		}

		Prism::RenderLine3D(pointA, pointB, GetActionColor(myCurrentAction));
	}

	if (myActions.Size() > 0)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(myCurrentActionPosition, 1.f);
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
	case ControllerComponent::eAction::ATTACK:
		return eColorDebug::RED;
		break;
	case ControllerComponent::eAction::RETURN:
		return eColorDebug::YELLOW;
		break;
	default:
		DL_ASSERT("Tried to GetActionColor of a invalid action");
		break;
	}

	return eColorDebug::NOT_USED;
}
