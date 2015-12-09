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

	eEntityType GetEntityToSpawn() const;
	const float& GetCurrentBuildTime() const;
	const float& GetMaxBuildTime() const;
	int GetSpawnQueueSize() const;

private:

	CU::StaticArray<eEntityType, 5> mySpawnQueue;
	int mySpawnQueueIndex;

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

inline eEntityType BuildingComponent::GetEntityToSpawn() const
{
	return mySpawnQueue[0];
}

inline const float& BuildingComponent::GetCurrentBuildTime() const
{
	return myCurrentBuildTime;
}

inline const float& BuildingComponent::GetMaxBuildTime() const
{
	return myMaxBuildTime;
}

inline int BuildingComponent::GetSpawnQueueSize() const
{
	return mySpawnQueueIndex + 1;
}