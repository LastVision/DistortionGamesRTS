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

	void Reset() override;

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void BuildUnit(eUnitType aUnitType);
	void UpgradeUnit(eUnitType aUnitType);

	eUnitType GetEntityToSpawn() const;
	const float& GetCurrentBuildTime() const;
	const float& GetMaxBuildTime() const;
	int GetSpawnQueueSize() const;
	const int& GetUnitCost(eUnitType aUnitType) const;
	const int& GetUnitSupplyCost(eUnitType aUnitType) const;
	int GetUpgradeCost(eUnitType aUnitType);
	const int& GetUpgradeCost(eUnitType aUnitType, int aUpgradeLevel) const;
	bool CanUpgrade(eUnitType aUnitType) const;

	void SetIgnoreBuildTime(bool anIgnoreBuildTime); // enables instabuilding

	bool IsQueueFull() const;

	const int& GetUpgradeLevel(int aUnitID) const;
	const float& GetUpgradeCooldown(int aUnitID) const;
	const float& GetUpgradeMaxCooldown(int aUnitID) const;

	void SetRallyPoint(const CU::Vector2<float>& aPoint);

private:
	struct BuildInfo
	{
		BuildInfo() : myUnit(eUnitType::NOT_A_UNIT), myIsUpgrade(false){}
		BuildInfo(eUnitType aUnit, bool aIsUpgrade) : myUnit(aUnit), myIsUpgrade(aIsUpgrade){}
		eUnitType myUnit;
		bool myIsUpgrade;
	};

	void UpdateUpgradeCooldown(float aDelta, eUnitType aUnit);

	CU::Vector2<float> mySpawnPoint;
	CU::Vector2<float> myRallyPoint;

	CU::StaticArray<int, 3> myUnitCosts;
	CU::StaticArray<float, 3> myUnitBuildTimes;
	CU::StaticArray<CU::StaticArray<Upgrade, 3>, 3> myUnitUpgrades; //3 upgrade-levels for all 3 units
	CU::StaticArray<int, 3> myUnitUpgradeProgress;
	CU::StaticArray<int, 3> myUnitSupplyCosts;
	CU::StaticArray<float, 3> myUpgradeCooldowns;
	CU::StaticArray<float, 3> myUpgradeMaxCooldowns;

	std::queue<BuildInfo> myBuildQueue;

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
	if (myBuildQueue.empty() == true)
	{
		return eUnitType::NOT_A_UNIT;
	}

	return myBuildQueue.front().myUnit;
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
	return myBuildQueue.size();
}

inline bool BuildingComponent::IsQueueFull() const
{
	return myBuildQueue.size() >= BUILD_QUEUE_SIZE;
}

inline void BuildingComponent::SetIgnoreBuildTime(bool anIgnoreBuildTime)
{
	myIgnoreBuildTime = anIgnoreBuildTime;
}

inline const int& BuildingComponent::GetUpgradeLevel(int aUnitID) const
{
	return myUnitUpgradeProgress[aUnitID];
}

inline const float& BuildingComponent::GetUpgradeCooldown(int aUnitID) const
{
	return myUpgradeCooldowns[aUnitID];
}

inline const float& BuildingComponent::GetUpgradeMaxCooldown(int aUnitID) const
{
	return myUpgradeMaxCooldowns[aUnitID];
}

inline void BuildingComponent::SetRallyPoint(const CU::Vector2<float>& aPoint)
{
	myRallyPoint = aPoint;
}