#include "stdafx.h"
#include "BuildingComponent.h"
#include "BuildingComponentData.h"
#include "Entity.h"
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <InputWrapper.h>

BuildingComponent::BuildingComponent(Entity& aEntity, BuildingCompnentData& aData)
	: Component(aEntity)
	, myBuildTypes(aData.myBuildUnitTypes)
	, myEntityToSpawn(eEntityType::EMPTY)
	, myCurrentBuildTime(0.f)
	, myMaxBuildTime(1.f)
{
}

BuildingComponent::~BuildingComponent()
{
}

void BuildingComponent::Update(float aDeltaTime)
{
	if (myEntityToSpawn != eEntityType::EMPTY)
	{
		myCurrentBuildTime += aDeltaTime;

		if (myCurrentBuildTime >= myMaxBuildTime)
		{
			PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(myEntityToSpawn, myEntity.GetOwner(), myEntity.GetScene()));
			myEntityToSpawn = eEntityType::EMPTY;
			myCurrentBuildTime = 0.f;
		}
	}
}

void BuildingComponent::BuildUnit(eEntityType aUnitType)
{
	if (myEntityToSpawn == eEntityType::EMPTY)
	{
		myEntityToSpawn = aUnitType;
		myCurrentBuildTime = 0.f;
	}
}
