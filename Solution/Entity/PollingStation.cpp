#include "stdafx.h"

//#include "CollisionComponent.h"
#include "PollingStation.h"

PollingStation* PollingStation::myInstance = nullptr;
PollingStation* PollingStation::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new PollingStation();
	}

	return myInstance;
}

void PollingStation::Destroy()
{
	SAFE_DELETE(myInstance);
}

void PollingStation::RegisterEntity(Entity* aEntity)
{
	switch (aEntity->GetType())
	{
	case eEntityType::UNIT:
	{
		switch (aEntity->GetOwner())
		{
		case eOwnerType::PLAYER:
			myPlayerUnits.Add(aEntity);
			break;
		case eOwnerType::ENEMY:
			myAIUnits.Add(aEntity);
			break;
		case eOwnerType::NEUTRAL:
			myNeutralUnits.Add(aEntity);
			break;
		default:
			DL_ASSERT("PollingStation tried to Register an Unit with invalid Owner");
			break;
		}
		break;
	}
	case eEntityType::RESOURCE_POINT:
		myResourcePoints.Add(aEntity);
		break;
	case eEntityType::VICTORY_POINT:
		myVictoryPoints.Add(aEntity);
		break;
	case eEntityType::BASE_BUILING:
		switch (aEntity->GetOwner())
		{
		case eOwnerType::PLAYER:
			myPlayerBuilding = aEntity;
			break;
		case eOwnerType::ENEMY:
			myAIBuilding = aEntity;
			break;
		default:
			DL_ASSERT("PollingStation tried to Register a Base with invalid Owner");
			break;
		}
		break;
		break;
	default:
		DL_ASSERT("PollingStation tried to Register an Entity with invalid Type");
		break;
	}
}

Entity* PollingStation::FindClosestEntity(const CU::Vector3<float>& aPosition, int aEntityOwner, float aMaxDistance2)
{
	float bestDist = FLT_MAX;
	Entity* bestEntity = nullptr;


	if ((aEntityOwner & eOwnerType::PLAYER) > 0)
	{
		FindClosestEntity(aPosition, myPlayerUnits, aMaxDistance2, bestDist, &bestEntity);
	}

	if ((aEntityOwner & eOwnerType::ENEMY) > 0)
	{
		FindClosestEntity(aPosition, myAIUnits, aMaxDistance2, bestDist, &bestEntity);
	}

	if ((aEntityOwner & eOwnerType::NEUTRAL) > 0)
	{
		FindClosestEntity(aPosition, myNeutralUnits, aMaxDistance2, bestDist, &bestEntity);
	}

	return bestEntity;
}

Entity* PollingStation::FindEntityAtPosition(const CU::Vector3<float>& aPosition, int aEntityOwner)
{
	return FindClosestEntity(aPosition, aEntityOwner, 10.f);
}

void PollingStation::FindAllEntitiesCloseToEntity(const Entity* anEntity, float aRadius
	, CU::GrowingArray<Entity*>& someEntitiesOut)
{
	FindAllEntitiesCloseToEntity(myPlayerUnits, anEntity, aRadius, someEntitiesOut);
	FindAllEntitiesCloseToEntity(myNeutralUnits, anEntity, aRadius, someEntitiesOut);
	FindAllEntitiesCloseToEntity(myAIUnits, anEntity, aRadius, someEntitiesOut);
}

void PollingStation::FindAllEntitiesCloseToEntity(const CU::GrowingArray<Entity*>& someEntitiesIn
	, const Entity* anEntity, float aRadius, CU::GrowingArray<Entity*>& someEntitiesOut)
{
	float dist2;
	for (int i = 0; i < someEntitiesIn.Size(); ++i)
	{
		Entity* current = someEntitiesIn[i];
		if (current->GetAlive() == true && current != anEntity)
		{
			dist2 = CU::Length2(current->GetPosition() - anEntity->GetPosition());

			if (dist2 < aRadius * aRadius)
			{
				someEntitiesOut.Add(current);
			}
		}
	}
}

const CU::GrowingArray<Entity*>& PollingStation::GetUnits(eOwnerType anOwner) const
{
	switch (anOwner)
	{
	case NOT_USED:
		DL_ASSERT("Can't get units for NOT USED enum.");
		break;
	case PLAYER:
		return myPlayerUnits;
		break;
	case ENEMY:
		return myAIUnits;
		break;
	case NEUTRAL:
		return myNeutralUnits;
		break;
	default:
		break;
	}

	DL_ASSERT("Owner type not found.");
	return myPlayerUnits;
}

const CU::GrowingArray<Entity*>& PollingStation::GetResourcePoints() const
{
	return myResourcePoints;
}

const CU::GrowingArray<Entity*>& PollingStation::GetVictoryPoints() const
{
	return myVictoryPoints;
}

const Entity* PollingStation::GetBase(eOwnerType anOwner) const
{
	if (anOwner == eOwnerType::PLAYER)
	{
		return myPlayerBuilding;
	}

	return myAIBuilding;
}

void PollingStation::CleanUp()
{
	for (int i = myPlayerUnits.Size() - 1; i >= 0; --i)
	{
		if (myPlayerUnits[i]->GetAlive() == false)
		{
			myPlayerUnits.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myAIUnits.Size() - 1; i >= 0; --i)
	{
		if (myAIUnits[i]->GetAlive() == false)
		{
			myAIUnits.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myNeutralUnits.Size() - 1; i >= 0; --i)
	{
		if (myNeutralUnits[i]->GetAlive() == false)
		{
			myNeutralUnits.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myResourcePoints.Size() - 1; i >= 0; --i)
	{
		if (myResourcePoints[i]->GetAlive() == false)
		{
			myResourcePoints.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myVictoryPoints.Size() - 1; i >= 0; --i)
	{
		if (myVictoryPoints[i]->GetAlive() == false)
		{
			myVictoryPoints.RemoveCyclicAtIndex(i);
		}
	}
}

PollingStation::PollingStation()
	: myPlayerUnits(64)
	, myAIUnits(64)
	, myNeutralUnits(64)
	, myResourcePoints(GC::resourcePointCount)
	, myVictoryPoints(GC::victoryPointCount)
{
}


PollingStation::~PollingStation()
{
}

void PollingStation::FindClosestEntity(const CU::Vector3<float>& aPosition
	, const CU::GrowingArray<Entity*>& someEntities, float aMaxDistance, float& aBestDistance, Entity** aOutEntity)
{
	float dist;
	for (int i = 0; i < someEntities.Size(); ++i)
	{
		if (someEntities[i]->GetAlive() == true)
		{
			dist = CU::Length2(someEntities[i]->GetOrientation().GetPos() - aPosition);

			if (dist < aBestDistance && dist < aMaxDistance)
			{
				*aOutEntity = someEntities[i];
				aBestDistance = dist;
			}
		}
	}
}
