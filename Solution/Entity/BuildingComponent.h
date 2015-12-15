#pragma once
#include "Component.h"
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
	void BuildUnit(eUnitType aUnitType);

	eUnitType GetEntityToSpawn() const;
	const float& GetCurrentBuildTime() const;
	const float& GetMaxBuildTime() const;
	int GetSpawnQueueSize() const;

	int GetUnitCost(eUnitType aUnitType);

	bool IsQueueFull() const;

private:

	CU::StaticArray<int, 3> myUnitCosts;

	CU::StaticArray<eUnitType, 5> mySpawnQueue;
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

inline eUnitType BuildingComponent::GetEntityToSpawn() const
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

inline bool BuildingComponent::IsQueueFull() const
{
	return mySpawnQueueIndex >= 4;
}