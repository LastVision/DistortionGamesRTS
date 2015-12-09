#include "stdafx.h"
#include "BuildingComponent.h"
#include "BuildingComponentData.h"
#include "Entity.h"
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <InputWrapper.h>

BuildingComponent::BuildingComponent(Entity& aEntity, BuildingCompnentData& aData)
	: Component(aEntity)
	, myCurrentBuildTime(0.f)
	, myMaxBuildTime(1.f)
	, mySpawnQueueIndex(-1)
{
	for (int i = 0; i <= 4; i++)
	{
		mySpawnQueue[i] = eEntityType::EMPTY;
	}
}

BuildingComponent::~BuildingComponent()
{
}

void BuildingComponent::Update(float aDeltaTime)
{
	if (mySpawnQueue[0] != eEntityType::EMPTY)
	{
		myCurrentBuildTime += aDeltaTime;

		if (myCurrentBuildTime >= myMaxBuildTime)
		{
			PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(mySpawnQueue[0], myEntity.GetOwner(), myEntity.GetScene()));

			for (int i = 0; i < mySpawnQueueIndex && i <= 3; i++)
			{
				mySpawnQueue[i] = mySpawnQueue[i + 1];
			}

			mySpawnQueue[mySpawnQueueIndex] = eEntityType::EMPTY;
			myCurrentBuildTime = 0.f;
			mySpawnQueueIndex--;
		}
	}
}

void BuildingComponent::BuildUnit(eEntityType aUnitType)
{
	if (mySpawnQueueIndex < 4)
	{
		mySpawnQueueIndex++;
		mySpawnQueue[mySpawnQueueIndex] = aUnitType;
	}
}
