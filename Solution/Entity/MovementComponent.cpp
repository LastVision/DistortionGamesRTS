#include "stdafx.h"
#include "Entity.h"
#include "MovementComponent.h"
#include "MovementComponentData.h"
#include <Terrain.h>
#include <XMLReader.h>

MovementComponent::MovementComponent(Entity& aEntity, MovementComponentData& aComponentData, Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myMovementSpeed(aComponentData.myMovementSpeed)
	, myTargetPosition(myEntity.myOrientation.GetPos())
	, myWayPoints(8)
	, myCurrentWayPoint(0)
	, myTerrain(aTerrain)
{

}

MovementComponent::~MovementComponent()
{
}

void MovementComponent::Update(float aDeltaTime)
{
	if (myEntity.GetState() == eEntityState::IDLE && myWayPoints.Size() > 0)
	{
		MoveTo(myWayPoints[myCurrentWayPoint]);
		++myCurrentWayPoint;
		if (myCurrentWayPoint >= myWayPoints.Size())
		{
			myWayPoints.RemoveAll();
			myCurrentWayPoint = 0;
		}
	}
	else if (myEntity.GetState() == eEntityState::WALKING)
	{
		CU::Vector3<float> position = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ position.x, 0.f, position.z });

		float dotResult = CU::Dot(myEntity.myOrientation.GetForward(), myTargetPosition - myEntity.myOrientation.GetPos());
		if (dotResult > 0)
		{
			CU::Vector3<float> newPos = myEntity.myOrientation.GetPos();
			newPos += myDirection * myMovementSpeed * aDeltaTime;
			myEntity.myOrientation.SetPos(newPos);
		}
		else
		{
			myEntity.myOrientation.SetPos(myTargetPosition);
			myDirection = { 0.f, 0.f, 0.f };
			myEntity.SetState(eEntityState::IDLE);
		}

		myTerrain.CalcEntityHeight(myEntity.myOrientation);
	}
}

void MovementComponent::SetWayPoints(const CU::GrowingArray<CU::Vector3<float>>& someWayPoints)
{
	myWayPoints.RemoveAll();
	for (int i = 0; i < someWayPoints.Size(); ++i)
	{
		myWayPoints.Add(someWayPoints[i]);
	}

	myCurrentWayPoint = 0;
	myEntity.SetState(eEntityState::IDLE);
}

void MovementComponent::MoveTo(CU::Vector3<float> aPointToMoveTo)
{
	myTargetPosition = { aPointToMoveTo };
	myDirection = CU::Vector3<float>(aPointToMoveTo) -myEntity.myOrientation.GetPos();
	CU::Normalize(myDirection);

	CU::Vector3<float> forward = myEntity.myOrientation.GetForward();
	CU::Normalize(forward);

	if (forward != myDirection)
{
		float angle = acos(CU::Dot(forward, myDirection));
		CU::Vector3<float> planeNormal(0, 1.f, 0);
		CU::Vector3<float> first(forward);
		CU::Vector3<float> second(myDirection);

		float dot = CU::Dot(first, second);
		float det = CU::Dot(planeNormal, CU::Cross(first, second));

		angle = atan2(det, dot);

		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ 0.f, 0.f, 0.f });
		myEntity.myOrientation = myEntity.myOrientation * CU::Matrix44<float>::CreateRotateAroundY(angle);
		myEntity.myOrientation.SetPos(pos);
	}

	myEntity.SetState(eEntityState::WALKING);
}