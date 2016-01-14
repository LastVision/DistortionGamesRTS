#pragma once
#include "Component.h"
#include <StaticArray.h>
#include <queue>
#include <GameStructs.h>

#define BUILD_QUEUE_SIZE 5
struct BuildingComponentData;



class BuildingComponent : public Component
{
public:
	BuildingComponent(Entity& aEntity, BuildingComponentData& aData);
	~BuildingComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void BuildUnit(eUnitType aUnitType);
	void UpgradeUnit(eUnitType aUnitType);

	eUnitType GetEntityToSpawn() const;
	const float& GetCurrentBuildTime() const;
	const float& GetMaxBuildTime() const;
	int GetSpawnQueueSize() const;
	int GetUnitCost(eUnitType aUnitType);
	int GetUnitSupplyCost(eUnitType aUnitType);
	int GetUpgradeCost(eUnitType aUnitType);
	bool CanUpgrade(eUnitType aUnitType);


	void SetIgnoreBuildTime(bool anIgnoreBuildTime); // enables instabuilding

	bool IsQueueFull() const;

	const int& GetUpgradeLevel(int aUnitID) const;

private:
	struct BuildInfo
	{
		BuildInfo() : myUnit(eUnitType::NOT_A_UNIT), myIsUpgrade(false){}
		BuildInfo(eUnitType aUnit, bool aIsUpgrade) : myUnit(aUnit), myIsUpgrade(aIsUpgrade){}
		eUnitType myUnit;
		bool myIsUpgrade;
	};

	CU::StaticArray<int, 3> myUnitCosts;
	CU::StaticArray<float, 3> myUnitBuildTimes;
	CU::StaticArray<CU::StaticArray<Upgrade, 3>, 3> myUnitUpgrades; //3 upgrade-levels for all 3 units
	CU::StaticArray<int, 3> myUnitUpgradeProgress;
	CU::StaticArray<int, 3> myUnitSupplyCosts;

	std::queue<BuildInfo> mySpawnQueue;

	float myCurrentBuildTime;
	float myMaxBuildTime;

	bool myIgnoreBuildTime;
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
	if (mySpawnQueue.empty() == true)
	{
		return eUnitType::NOT_A_UNIT;
	}

	return mySpawnQueue.front().myUnit;
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
	return mySpawnQueue.size();
}

inline bool BuildingComponent::IsQueueFull() const
{
	return mySpawnQueue.size() >= BUILD_QUEUE_SIZE;
}

inline void BuildingComponent::SetIgnoreBuildTime(bool anIgnoreBuildTime)
{
	myIgnoreBuildTime = anIgnoreBuildTime;
}

inline const int& BuildingComponent::GetUpgradeLevel(int aUnitID) const
{
	return myUnitUpgradeProgress[aUnitID];
}