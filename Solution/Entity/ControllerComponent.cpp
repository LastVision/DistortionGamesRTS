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
{
	DL_ASSERT_EXP(myEntity.GetComponent<ActorComponent>() != nullptr
		, "ControllerComponent wont work without a ActorComponent");
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Update(float aDelta)
{
	if (myCurrentAction == eAction::MOVE)
	{
		DoMoveAction(aDelta);
	}
	else if (myCurrentAction == eAction::ATTACK)
	{
		float distToTarget = CU::Length2(myAttackTarget->GetOrientation().GetPos() - myEntity.GetOrientation().GetPos());

		if (distToTarget < myAttackRange)
		{
			DoAttackAction();
		}
		else
		{
			myWayPoints.RemoveAll();
			myEntity.SetState(eEntityState::IDLE);
			myWayPoints.Add(myAttackTarget->GetOrientation().GetPos());

			DoMoveAction(aDelta);
		}
	}
	else if (myCurrentAction == eAction::ATTACK_MOVE)
	{
		Entity* enemyInRange = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
			, eOwnerType::ENEMY, myVisionRange);
		if (enemyInRange != nullptr)
		{
			float distToTarget = CU::Length2(myAttackTarget->GetOrientation().GetPos() - myEntity.GetOrientation().GetPos());

			if (distToTarget < myAttackRange)
			{
				DoAttackAction();
			}
			else if (distToTarget < myVisionRange)
			{
				myChaseOrigin = myEntity.GetOrientation().GetPos();
				myCurrentAction = eAction::CHASE;
				myAttackTarget = enemyInRange;
			}
			else
			{
				myWayPoints.RemoveAll();
				myEntity.SetState(eEntityState::IDLE);
				myWayPoints.Add(myAttackTarget->GetOrientation().GetPos());

				DoMoveAction(aDelta);
			}
		}
		else
		{
			DoMoveAction(aDelta);
		}
	}
	else if (myCurrentAction == eAction::CHASE)
	{
		myWayPoints.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);
		myWayPoints.Add(myAttackTarget->GetOrientation().GetPos());

		DoMoveAction(aDelta);

		float distFromChaseOrigin = CU::Length2(myAttackTarget->GetOrientation().GetPos() - myChaseOrigin);

	}


	//if (myCurrentAction == eAction::ATTACK_MOVE)
	//{
	//	Entity* enemyInVision = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
	//		, eOwnerType::ENEMY, myVisionRange);

	//	if (enemyInVision != nullptr)
	//	{
	//		//Start Moving towards enemy
	//		myAttackTarget = enemyInVision;
	//		myChaseOrigin = myEntity.GetOrientation().GetPos();
	//		myCurrentAction = eAction::CHASE;
	//	}
	//	else
	//	{
	//		//Keep moving towards the clickposition
	//		DoMoveAction(aDelta);
	//	}
	//}


	if (myEntity.GetState() == eEntityState::WALKING)
	{
		CU::Vector3<float> targetPosition = myTerrain.GetHeight(myCurrentWayPoint, 2.f);
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
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearPreviousPoints)
{
	if (aClearPreviousPoints == true)
	{
		myWayPoints.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);
	}

	myWayPoints.Add(aPosition);

	myAttackTarget = nullptr;
	myCurrentAction = eAction::MOVE;
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition)
{
	myWayPoints.RemoveAll();
	myEntity.SetState(eEntityState::IDLE);

	myWayPoints.Add(aPosition);
	myAttackTarget = nullptr;
	myCurrentAction = eAction::ATTACK_MOVE;

	myLastMoveTarget = aPosition;
}

void ControllerComponent::Attack(Entity* aTarget)
{
	if (myEntity.GetState() != eEntityState::ATTACKING || myAttackTarget != aTarget)
	{
		myWayPoints.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);

		myAttackTarget = aTarget;
		myCurrentAction = eAction::ATTACK;
	}
}

void ControllerComponent::DoMoveAction(float)
{
	if (myEntity.GetState() == eEntityState::IDLE && myWayPoints.Size() > 0)
	{
		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		myCurrentWayPoint = myWayPoints[0];
		myData.myDirection = myCurrentWayPoint - pos;
		myWayPoints.RemoveNonCyclicAtIndex(0);
	}
	else if (myEntity.GetState() == eEntityState::WALKING)
	{
		CU::Vector3<float> position = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ position.x, 0.f, position.z });

		float dotResult = CU::Dot(myEntity.myOrientation.GetForward(), myCurrentWayPoint - myEntity.myOrientation.GetPos());
		if (dotResult <= 0)
		{
			myEntity.myOrientation.SetPos(myCurrentWayPoint);
			myData.myDirection = { 0.f, 0.f, 0.f };
			myEntity.SetState(eEntityState::IDLE);
		}

		myEntity.myOrientation.SetPos(position);
	}
}

void ControllerComponent::DoAttackAction()
{
	if (myEntity.GetState() == eEntityState::ATTACKING)
	{
		AnimationComponent* animation = myEntity.GetComponent<AnimationComponent>();
		if (animation != nullptr && animation->IsCurrentAnimationDone())
		{
			bool restartAnimation = true;

			HealthComponent* targetHealth = myAttackTarget->GetComponent<HealthComponent>();
			if (targetHealth != nullptr)
			{
				restartAnimation = targetHealth->TakeDamage(5);
			}

			if (restartAnimation == true)
			{
				animation->RestartCurrentAnimation();
			}
		}
	}
	else
	{
		myEntity.SetState(eEntityState::ATTACKING);
		myData.myDirection = { 0.f, 0.f, 0.f };
	}
}