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
{
}

BuildingComponent::~BuildingComponent()
{
}

void BuildingComponent::BuildUnit(eEntityType aUnitType)
{
	PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(aUnitType, myEntity.GetOwner(), myEntity.GetScene()));
}
