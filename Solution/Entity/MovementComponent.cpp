#include "stdafx.h"
#include "Entity.h"
#include "MovementComponent.h"
#include "MovementComponentData.h"
#include <Terrain.h>
#include <XMLReader.h>

MovementComponent::MovementComponent(Entity& aEntity, MovementComponentData& aComponentData, const Prism::Terrain& aTerrain)
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
		MoveTo(myWayPoints[0]);
		myWayPoints.RemoveNonCyclicAtIndex(0);
		/*++myCurrentWayPoint;
		if (myCurrentWayPoint >= myWayPoints.Size())
		{
			myWayPoints.RemoveAll();
			myCurrentWayPoint = 0;
		}*/
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


	if (myEntity.GetState() == eEntityState::WALKING)
	{
		CU::Vector3<float> targetPosition = myTerrain.GetHeight(myTargetPosition, 2.f);
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

void MovementComponent::AddWayPoints(const CU::GrowingArray<CU::Vector3<float>>& someWayPoints, bool aRemovePreviousPoints)
{
	if (aRemovePreviousPoints == true)
	{
		myWayPoints.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);
		myCurrentWayPoint = 0;
	}

	for (int i = 0; i < someWayPoints.Size(); ++i)
	{
		myWayPoints.Add(someWayPoints[i]);
	}
}

void MovementComponent::AddWayPoint(const CU::Vector3<float>& aWayPoint, bool aRemovePreviousPoints)
{
	if (aRemovePreviousPoints == true)
	{
		myWayPoints.RemoveAll();
		myEntity.SetState(eEntityState::IDLE);
		myCurrentWayPoint = 0;
	}

	myWayPoints.Add(aWayPoint);
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
		CU::Vector3<float> planeNormal(0, 1.f, 0);
		float dot = CU::Dot(forward, myDirection);
		float det = CU::Dot(planeNormal, CU::Cross(forward, myDirection));

		float angle = atan2(det, dot);

		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ 0.f, 0.f, 0.f });
		myEntity.myOrientation = myEntity.myOrientation * CU::Matrix44<float>::CreateRotateAroundY(angle);
		myEntity.myOrientation.SetPos(pos);
	}

	myEntity.SetState(eEntityState::WALKING);
}