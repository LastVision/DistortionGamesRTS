#include "stdafx.h"

#include "ActorComponent.h"
#include <ArtifactMessage.h>
//#include "ActorComponentData.h"
#include "ControllerComponent.h"
#include <Terrain.h>
#include "AnimationComponent.h"
#include "PollingStation.h"
#include <PostMaster.h>

ActorComponent::ActorComponent(Entity& aEntity, ActorComponentData& aData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myTerrain(aTerrain)
{
	myEntity.myMaxSpeed = aData.myMoveSpeed;
}

ActorComponent::~ActorComponent()
{
}

void ActorComponent::Update(float aDelta)
{
	//if (myEntity.GetAlive() == false || myEntity.GetState() == eEntityState::DYING)
	//{
	//	return;
	//}

	const CU::Vector2<float>& acceleration = myEntity.GetComponent<ControllerComponent>()->GetAcceleration();

	myEntity.myVelocity += acceleration * aDelta;
	float velocity2 = CU::Length2(myEntity.myVelocity);
	if (velocity2 > myEntity.myMaxSpeed * myEntity.myMaxSpeed)
	{
		CU::Normalize(myEntity.myVelocity);
		myEntity.myVelocity *= myEntity.myMaxSpeed;
	}
	switch (myEntity.GetIntention())
	{
	case eEntityState::ATTACKING:
		myEntity.SetState(eEntityState::ATTACKING);
		break;
	case eEntityState::DYING:
		myEntity.SetState(eEntityState::DYING);
		break;
	case eEntityState::IDLE:
		if (velocity2 > 1.f || CU::Length2(acceleration) > 1.f)
		{
			//CU::Normalize(direction);
			LookInDirection(myEntity.myVelocity);
			myEntity.SetState(eEntityState::WALKING);

			CU::Vector2<float> position = myEntity.myPosition;

			position += myEntity.myVelocity * aDelta;
			myEntity.myOrientation.SetPos({ position.x, 0, position.y });
			myTerrain.CalcEntityHeight(myEntity.myOrientation);
		}
		else
		{
			myEntity.GetComponent<AnimationComponent>()->PlayAnimation(eEntityState::IDLE);
			myEntity.SetState(eEntityState::IDLE);
		}
		break;
	case eEntityState::WALKING:
		if (velocity2 > 1.f || CU::Length2(acceleration) > 1.f)
		{
			//CU::Normalize(direction);
			LookInDirection(myEntity.myVelocity);
			myEntity.SetState(eEntityState::WALKING);
			myEntity.GetComponent<AnimationComponent>()->PlayAnimation(eEntityState::WALKING);
			CU::Vector2<float> position = myEntity.myPosition;

			position += myEntity.myVelocity * aDelta;
			myEntity.myOrientation.SetPos({ position.x, 0, position.y });
			myTerrain.CalcEntityHeight(myEntity.myOrientation);
		}
		break;
	default:
		DL_ASSERT("Unknown intentionstate!");
		break;
	}

	//if (velocity2 > 1.f || CU::Length2(acceleration) > 1.f)
	//{
	//	//CU::Normalize(direction);
	//	LookInDirection(myEntity.myVelocity);
	//	myEntity.SetState(eEntityState::WALKING);
	//
	//	CU::Vector2<float> position = myEntity.myPosition;
	//
	//	position += myEntity.myVelocity * aDelta;
	//	myEntity.myOrientation.SetPos({ position.x, 0, position.y });
	//	myTerrain.CalcEntityHeight(myEntity.myOrientation);
	//}
	//else if (myEntity.GetState() != eEntityState::ATTACKING)
	//{
	//	myEntity.SetState(eEntityState::IDLE);
	//}



	if (myEntity.GetOwner() != eOwnerType::NEUTRAL)
	{
		const CU::GrowingArray<Entity*>& artifacts = PollingStation::GetInstance()->GetArtifacts();

		for (int i = artifacts.Size() - 1; i >= 0; --i)
		{
			if (CU::Length2(myEntity.GetPosition() - artifacts[i]->GetPosition()) < 5 * 5)
			{
				PostMaster::GetInstance()->SendMessage(ArtifactMessage(myEntity.GetOwner(), 1));
				PollingStation::GetInstance()->DeleteArtifact(artifacts[i]);
			}
		}
	}
}

void ActorComponent::LookInDirection(const CU::Vector2<float>& aDirection)
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

		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ 0.f, 0.f, 0.f });
		myEntity.myOrientation = myEntity.myOrientation * CU::Matrix44<float>::CreateRotateAroundY(angle);
		myEntity.myOrientation.SetPos(pos);
	}
}

void ActorComponent::LookAtPoint(const CU::Vector2<float>& aPoint)
{
	CU::Vector2<float> direction = aPoint - myEntity.myPosition;
	LookInDirection(direction);
}