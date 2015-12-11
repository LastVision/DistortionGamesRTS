#include "stdafx.h"

//#include "CollisionComponent.h"
#include "Entity.h"
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
	switch (aEntity->GetOwner())
	{
	case eOwnerType::PLAYER:
		myPlayerUnits.Add(aEntity);
		break;
	case eOwnerType::ENEMY:
		myAIUnits.Add(aEntity);
		break;
	default:
		DL_ASSERT("PollingStation tried to Register an Entity with unknown Owner");
		break;
	}
}

Entity* PollingStation::FindClosestEntity(const CU::Vector3<float>& aPosition, eOwnerType aEntityOwner, float aMaxDistance)
{
	float bestDist = FLT_MAX;
	float dist = 0;
	Entity* entity = nullptr;
	CU::GrowingArray<Entity*>* list = nullptr;
	switch (aEntityOwner)
	{
	case PLAYER:
		list = &myPlayerUnits;
		break;
	case ENEMY:
		list = &myAIUnits;
		break;
	default:
		DL_ASSERT("PollingStation tried to FindClosestEntity of an unknown type");
		break;
	}

	for (int i = 0; i < (*list).Size(); ++i)
	{
		if ((*list)[i]->GetAlive() == true)
		{
			dist = CU::Length2((*list)[i]->GetOrientation().GetPos() - aPosition);

			if (dist < bestDist && dist < aMaxDistance)
			{
				bestDist = dist;
				entity = (*list)[i];
			}
		}
	}


	return entity;
}

Entity* PollingStation::FindEntityAtPosition(const CU::Vector3<float>& aPosition, eOwnerType aEntityOwner)
{
	Entity* entity = nullptr;
	//CollisionComponent* collision = nullptr;

	//if (aEntityOwner == eOwnerType::PLAYER)
	//{
	//	for (int i = 0; i < myPlayerUnits.Size(); ++i)
	//	{
	//		if (myPlayerUnits[i]->GetAlive() == true)
	//		{
	//			collision = myPlayerUnits[i]->GetComponent<CollisionComponent>();
	//			if (collision != nullptr && collision->Collide(aPosition))
	//			{
	//				entity = myPlayerUnits[i];
	//				break;
	//			}
	//		}
	//	}
	//}
	//else if (aEntityOwner == eOwnerType::ENEMY)
	//{
	//	for (int i = 0; i < myAIUnits.Size(); ++i)
	//	{
	//		if (myAIUnits[i]->GetAlive() == true)
	//		{
	//			collision = myAIUnits[i]->GetComponent<CollisionComponent>();
	//			if (collision != nullptr && collision->Collide(aPosition))
	//			{
	//				entity = myAIUnits[i];
	//				break;
	//			}
	//		}
	//	}
	//}

	return entity;
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
		DL_ASSERT("Not implemented yet.");
		break;
	default:
		break;
	}

	DL_ASSERT("Owner type not found.");
	return myPlayerUnits;
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
}

PollingStation::PollingStation()
	: myPlayerUnits(64)
	, myAIUnits(64)
{
}


PollingStation::~PollingStation()
{
}
