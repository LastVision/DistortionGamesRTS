#include "stdafx.h"

#include <Entity.h>
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
	delete myInstance;
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

Entity* PollingStation::FindClosestEntity(const CU::Vector3<float>& aPosition, eOwnerType aEntityOwner)
{
	float bestDist = FLT_MAX;
	float dist = 0;
	Entity* entity = nullptr;
	if (aEntityOwner == eOwnerType::PLAYER)
	{
		for (int i = 0; i < myPlayerUnits.Size(); ++i)
		{
			dist = CU::Length2(myPlayerUnits[i]->GetOrientation().GetPos() - aPosition);

			if (dist < bestDist)
			{
				bestDist = dist;
				entity = myPlayerUnits[i];
			}
		}
	}
	else if (aEntityOwner == eOwnerType::ENEMY)
	{
		for (int i = 0; i < myAIUnits.Size(); ++i)
		{
			dist = CU::Length2(myAIUnits[i]->GetOrientation().GetPos() - aPosition);

			if (dist < bestDist)
			{
				bestDist = dist;
				entity = myAIUnits[i];
			}
		}
	}
	else
	{
		DL_ASSERT("PollingStation tried to FindClosestEntity of an unknown type");
	}

	DL_ASSERT_EXP(entity != nullptr, "PollingStation failed to FindClosestEneity");
	return entity;
}

PollingStation::PollingStation()
	: myPlayerUnits(64)
	, myAIUnits(64)
{
}


PollingStation::~PollingStation()
{
}
