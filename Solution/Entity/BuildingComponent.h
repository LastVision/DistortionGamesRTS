#pragma once
#include "Component.h"
#include "EntityEnum.h"
#include <StaticArray.h>

struct BuildingCompnentData;

class BuildingComponent : public Component
{
public:
	BuildingComponent(Entity& aEntity, BuildingCompnentData& aData);
	~BuildingComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void BuildUnit(eEntityType aUnitType);
private:

	CU::StaticArray<eEntityType, 3> myBuildTypes;

	eEntityType myEntityToSpawn;

	float myCurrentBuildTime;
	float myMaxBuildTime;
};

inline eComponentType BuildingComponent::GetTypeStatic()
{
	return eComponentType::BUILDING;
}

inline eComponentType BuildingComponent::GetType()
{
	return GetTypeStatic();
}
