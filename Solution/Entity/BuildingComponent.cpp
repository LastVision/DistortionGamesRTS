#include "stdafx.h"
#include "BuildingComponent.h"
#include <PostMaster.h>
#include <SpawnUnitMessage.h>

BuildingComponent::BuildingComponent(Entity& aEntity, BuildingComponentData& aData)
	: Component(aEntity)
	, myCurrentBuildTime(0.f)
	, myMaxBuildTime(2.f)
	, mySpawnQueueIndex(-1)
	, myUnitCosts(aData.myUnitCosts)
	, myUnitBuildTimes(aData.myUnitBuildTimes)
	, myIgnoreBuildTime(false)
{
	for (int i = 0; i <= 4; i++)
	{
		mySpawnQueue[i] = eUnitType::NOT_A_UNIT;
	}
}

BuildingComponent::~BuildingComponent()
{
}

void BuildingComponent::Update(float aDeltaTime)
{
	if (mySpawnQueue[0] != eUnitType::NOT_A_UNIT)
	{
		myMaxBuildTime = myIgnoreBuildTime ? 0.f : myUnitBuildTimes[static_cast<int>(mySpawnQueue[0])];

		myCurrentBuildTime += aDeltaTime;

		if (myCurrentBuildTime >= myMaxBuildTime)
		{
			PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(mySpawnQueue[0], myEntity.GetOwner()));

			for (int i = 0; i < mySpawnQueueIndex && i <= 3; i++)
			{
				mySpawnQueue[i] = mySpawnQueue[i + 1];
			}

			mySpawnQueue[mySpawnQueueIndex] = eUnitType::NOT_A_UNIT;
			myCurrentBuildTime = 0.f;
			mySpawnQueueIndex--;
		}
	}
	else
	{
		myMaxBuildTime = 0.f;
	}
}

void BuildingComponent::BuildUnit(eUnitType aUnitType)
{
	if (mySpawnQueueIndex < 4)
	{
		mySpawnQueueIndex++;
		mySpawnQueue[mySpawnQueueIndex] = aUnitType;
	}
}

int BuildingComponent::GetUnitCost(eUnitType aUnitType)
{
	return myUnitCosts[static_cast<int>(aUnitType)];
}