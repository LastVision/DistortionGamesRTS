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

	if (myEntity.GetState() == eEntityState::IDLE)
	{
		StartNextAction();
		}

	//if (myCurrentAction.myAction == eAction::IDLE)
	//{
	//	myEntity.SetState(eEntityState::IDLE);
	//	if (myOwnerType == eOwnerType::NEUTRAL)
	//	{
	//		int apa = 5;
	//	}
	//	Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
	//		, myTargetType, myVisionRange2);

	//	if (closeTarget != nullptr)
	//	{
	//		myAttackTarget = closeTarget;
	//		myReturnPosition = myEntity.myPosition;

	//		ActionData action;
	//		action.myAction = eAction::ATTACK_TARGET;
	//		action.myPosition = myAttackTarget->myPosition;
	//		myActions.Add(action);
	//	}
	//}
	//else if (myCurrentAction.myAction == eAction::ATTACK_MOVE)
	//{
	//	if (myAttackTarget != nullptr)
	//	{
	//		DoAttackAction();
	//	}
	//	else
	//	{
	//		DoMoveAction(myCurrentAction.myPosition);

	//		myAttackTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
	//			, myTargetType, myVisionRange2 * 10.f);
	//	}
	//}
	//else if (myCurrentAction.myAction == eAction::RETURN)
	//{
	//	DL_ASSERT_EXP(myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX
	//		, "ControllerComponent tried to do RETURN-Action without a valid ReturnPosition");

	//	Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity({ myReturnPosition.x, 0, myReturnPosition.y }
	//		, myTargetType, myVisionRange2);

	//	if (closeTarget != nullptr)
	//	{
	//		myAttackTarget = closeTarget;
	//		myCurrentAction.myAction = eAction::ATTACK_TARGET;
	//	}
	//	else
	//	{
	//		if (myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX)
	//		{
	//			DoMoveAction(myReturnPosition);
	//		}
	//		else
	//		{
	//			myReturnPosition = myEntity.myPosition;
	//		}
	//	}
	//}
	//else if (myCurrentAction.myAction == eAction::HOLD)
	//{
	//	Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
	//		, myTargetType, myAttackRange2);

	//	if (closeTarget != nullptr)
	//	{
	//		myAttackTarget = closeTarget;
	//		AttackTarget();
	//	}
	//}
	//else if (myCurrentAction.myAction == eAction::ATTACK_TARGET)
	//{
	//	myCurrentAttackTargetPathRefreshTime -= aDelta;
	//	if (myCurrentAttackTargetPathRefreshTime <= 0.f)
	//	{
	//		RefreshPathToAttackTarget();
	//	}

	//	Entity* closeTarget = PollingStation::GetInstance()->FindClosestEntity(myEntity.GetOrientation().GetPos()
	//		, myTargetType, myAttackRange2);

	//	if (closeTarget != nullptr && closeTarget == myAttackTarget)
	//	{
	//		AttackTarget();
	//	}
	//	else
	//	{
	//		DoMoveAction(myCurrentAction.myPosition);
	//	}
	//}
	//else if (myCurrentAction.myAction == eAction::MOVE)
	//{
	//	//DoMoveAction(myCurrentAction.myPosition);
	//}

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



	myData.myAcceleration = myBehavior->Update();

	RenderDebugLines();
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	FillCommandList(eAction::MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::AttackTarget(Entity* aEntity, bool aClearCommandQueue)
{
	FillCommandList(eAction::ATTACK_TARGET, aClearCommandQueue, aEntity);
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue)
{
	FillCommandList(eAction::ATTACK_MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });

	//Entity* target = nullptr;
	//switch (myEntity.GetOwner())
	//{
	//case eOwnerType::PLAYER:
	//	target = PollingStation::GetInstance()->FindEntityAtPosition(aPosition, eOwnerType::ENEMY);
	//	break;
	//case eOwnerType::ENEMY:
	//	target = PollingStation::GetInstance()->FindEntityAtPosition(aPosition, eOwnerType::PLAYER);
	//	break;
	//default:
	//	DL_ASSERT("An Entity thats not owned by Player or AI tried to Attack");
	//	break;
	//}

	//myReturnPosition.x = FLT_MAX;
	//myReturnPosition.y = FLT_MAX;

	//if (myEntity.GetState() != eEntityState::ATTACKING || myAttackTarget != target)
	//{
	//	myAttackTarget = target;
	//}
	}

void ControllerComponent::Stop()
{
	myActions.RemoveAll();
	myCurrentAction.myAction = eAction::IDLE;
	myCurrentAction.myPosition = myEntity.myPosition;
	myData.myAcceleration = { 0.f, 0.f };
}

void ControllerComponent::HoldPosition()
{
	myActions.RemoveAll();
	myCurrentAction.myAction = eAction::HOLD;
	myCurrentAction.myPosition = myEntity.myPosition;
	myData.myAcceleration = { 0.f, 0.f };
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
	action.myEntity = aEntity;
	action.myPosition = aTargetPosition;
	if (myActions.Size() > 0)
	{
		myActions.Insert(0, action);
		}
		else
		{
			myActions.Add(action);
		}
	//CU::GrowingArray<CU::Vector3<float>> path(16);
	//if (myTerrain.GetPathFinder()->FindPath(myEntity.GetOrientation().GetPos(), { aTargetPosition.x, 0, aTargetPosition.y }, path) == true)
	//{
	//	if (path.Size() > 0)
	//	{
	//		for (int i = path.Size() - 1; i >= 0; --i)
	//		{
	//			CU::Vector2<float> target = { path[i].x, path[i].z };

	//			action.myPosition = target;

	//			myActions.Add(action);
	//		}
	//	}
	//	else
	//	{
	//		action.myPosition = aTargetPosition;
	//		myActions.Add(action);
	//	}
	//}

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

	
	if (CU::Length2(myCurrentAction.myEntity->GetPosition() - myEntity.GetPosition()) < myAttackRange2)
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

//void ControllerComponent::DoAttack()
//{
//	bool shouldReturn = false;
//	bool hasReturnPosition = false;
//	float distanceToTarget = CU::Length2(myEntity.GetOrientation().GetPos() - myAttackTarget->GetOrientation().GetPos());
//	if (myReturnPosition.x != FLT_MAX && myReturnPosition.y != FLT_MAX)
//	{
//		float distanceToReturnPosition = CU::Length2(myEntity.myPosition - myReturnPosition);
//		hasReturnPosition = true;
//		float chaseDistance = myChaseDistance2;
//
//		if (myEntity.GetOwner() == eOwnerType::NEUTRAL)
//		{
//			chaseDistance = myChaseDistanceNeutral2;
//		}
//
//		if (distanceToReturnPosition > chaseDistance)
//		{
//			shouldReturn = true;
//		}
//	}
//
//	if (distanceToTarget < myAttackRange2 && shouldReturn == false)
//	{
//		AttackTarget();
//		if (myAttackTarget->GetAlive() == false)
//		{
//			if (hasReturnPosition == true)
//			{
//				myCurrentAction.myAction = eAction::RETURN;
//				myEntity.GetComponent<ActorComponent>()->LookAtPoint(myReturnPosition);
//				myData.myAcceleration = myReturnPosition - myEntity.myPosition;					//should be in behavior
//			}
//			else
//			{
//				myAttackTarget = nullptr;
//			}
//		}
//	}
//	else if (shouldReturn == true)
//	{
//		myCurrentAction.myAction = eAction::RETURN;
//		myEntity.GetComponent<ActorComponent>()->LookAtPoint(myReturnPosition);
//		myData.myAcceleration = myReturnPosition - myEntity.myPosition;
//	}
//	else
//	{
//		DoMoveAction(myAttackTarget->myPosition);
//	}
//}

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
		}
		
		HealthComponent* targetHealth = myCurrentAction.myEntity->GetComponent<HealthComponent>();
		bool targetSurvived = false;
		if (targetHealth != nullptr && myCurrentAction.myEntity->GetAlive())
		{
			targetSurvived = targetHealth->TakeDamage(myAttackDamage);
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

		//switch (myCurrentAction.myAction)
		//{
		//	case eAction::IDLE:
		//		DL_ASSERT("Can't have idle in action queue..?");
		//		break;
		//	case eAction::MOVE:
		//		myEntity.SetState(eEntityState::WALKING);
		//		break;
		//	case eAction::ATTACK_TARGET:
		//		myEntity.SetState(eEntityState::ATTACKING);
		//		break;
		//	case eAction::RETURN:
		//		myEntity.SetState(eEntityState::ATTACKING);
		//		break;
		//	case eAction::HOLD:
		//		myEntity.SetState(eEntityState::ATTACKING);
		//		break;
		//	case eAction::ATTACK_MOVE:
		//		myEntity.SetState(eEntityState::ATTACKING);
		//		break;
		//}

		//if (myCurrentAction.myAction == eAction::ATTACK_MOVE
		//	|| myCurrentAction.myAction == eAction::ATTACK_TARGET
		//	|| myCurrentAction.myAction == eAction::MOVE
		//	|| myCurrentAction.myAction == eAction::RETURN)
		//{
		//	myEntity.SetState(eEntityState::WALKING);
		//}
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
			color = GetActionColor(myActions[i-1].myAction);


			pointA = myTerrain.GetHeight(GetPosition(myActions[i]), 1.f);
			pointB = myTerrain.GetHeight(GetPosition(myActions[i-1]), 1.f);
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
	myData.myAcceleration = { 0.f, 0.f };
	myBehavior->SetTarget({ myEntity.GetPosition().x + CU::Math::RandomRange<float>(-0.1f, 0.1f), myEntity.GetPosition().y - 2.f });
}