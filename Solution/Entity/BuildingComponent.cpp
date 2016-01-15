#include "stdafx.h"
#include "BuildingComponent.h"
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <UpgradeUnitMessage.h>

BuildingComponent::BuildingComponent(Entity& aEntity, BuildingComponentData& aData)
	: Component(aEntity)
	, myCurrentBuildTime(0.f)
	, myMaxBuildTime(2.f)
	, myUnitCosts(aData.myUnitCosts)
	, myUnitBuildTimes(aData.myUnitBuildTimes)
	, myIgnoreBuildTime(false)
	, myUnitUpgrades(aData.myUnitUpgrades)
	, myUnitSupplyCosts(aData.myUnitSupplyCosts)
{
	for (int i = 0; i < 3; ++i)
	{
		myUnitUpgradeProgress[i] = 0;

		myUpgradeCooldowns[i] = myUnitUpgrades[i][0].myCooldown;
	}
}

BuildingComponent::~BuildingComponent()
{
}

void BuildingComponent::Update(float aDeltaTime)
{
	

	UpdateUpgradeCooldown(aDeltaTime, eUnitType::GRUNT);
	UpdateUpgradeCooldown(aDeltaTime, eUnitType::RANGER);
	UpdateUpgradeCooldown(aDeltaTime, eUnitType::TANK);

	if (mySpawnQueue.empty() == true)
	{
		return;
	}

	const BuildInfo& currentOrder = mySpawnQueue.front();
	if (currentOrder.myUnit == eUnitType::NOT_A_UNIT)
	{
		return;
	}

	int unitIndex = static_cast<int>(currentOrder.myUnit);
	int upgradeIndex = myUnitUpgradeProgress[unitIndex];

	if (currentOrder.myIsUpgrade == true)
	{
		myMaxBuildTime = myIgnoreBuildTime ? 0.f : myUnitUpgrades[unitIndex][upgradeIndex].myBuildTime;
	}
	else
	{
		myMaxBuildTime = myIgnoreBuildTime ? 0.f : myUnitBuildTimes[unitIndex];
	}

	myCurrentBuildTime += aDeltaTime;
	if (myCurrentBuildTime >= myMaxBuildTime)
	{
		if (currentOrder.myIsUpgrade == true)
		{
			PostMaster::GetInstance()->SendMessage(UpgradeUnitMessage(currentOrder.myUnit, myUnitUpgrades[unitIndex][upgradeIndex]
				, myEntity.GetOwner()));
			++myUnitUpgradeProgress[unitIndex];
			upgradeIndex = myUnitUpgradeProgress[unitIndex];
			myUpgradeCooldowns[unitIndex] = myUnitUpgrades[unitIndex][upgradeIndex].myCooldown;
		}
		else
		{
			PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(currentOrder.myUnit, myEntity.GetOwner(), myEntity.GetPosition() + CU::Vector2<float>(0, -15.f)));
		}

		mySpawnQueue.pop();
		myCurrentBuildTime = 0.f;
	}
}

void BuildingComponent::BuildUnit(eUnitType aUnitType)
{
	if (mySpawnQueue.size() < BUILD_QUEUE_SIZE)
	{
		mySpawnQueue.push({ aUnitType, false });
	}
}

void BuildingComponent::UpgradeUnit(eUnitType aUnitType)
{
	if (mySpawnQueue.size() < BUILD_QUEUE_SIZE)
	{
		mySpawnQueue.push({ aUnitType, true });
		int unitIndex = static_cast<int>(aUnitType);
		int upgradeIndex = myUnitUpgradeProgress[unitIndex];
		myUnitUpgrades[unitIndex][upgradeIndex].myInProgress = true;
	}
}

int BuildingComponent::GetUnitCost(eUnitType aUnitType)
{
	return myUnitCosts[static_cast<int>(aUnitType)];
}

int BuildingComponent::GetUnitSupplyCost(eUnitType aUnitType)
{
	return myUnitSupplyCosts[static_cast<int>(aUnitType)];
}

int BuildingComponent::GetUpgradeCost(eUnitType aUnitType)
{
	int unitIndex = static_cast<int>(aUnitType);
	int upgradeIndex = myUnitUpgradeProgress[unitIndex];
	return myUnitUpgrades[unitIndex][upgradeIndex].myCost;
}

bool BuildingComponent::CanUpgrade(eUnitType aUnitType)
{
	int unitIndex = static_cast<int>(aUnitType);
	int upgradeIndex = myUnitUpgradeProgress[unitIndex];

	if (myUnitUpgrades[unitIndex][upgradeIndex].myInProgress == true)
	{
		return false;
	}

	if (myUpgradeCooldowns[unitIndex] > 0.f)
	{
		return false;
	}

	return upgradeIndex < 3;
}

void BuildingComponent::UpdateUpgradeCooldown(float aDelta, eUnitType aUnit)
{
	int unitIndex = static_cast<int>(aUnit);

	myUpgradeCooldowns[unitIndex] -= aDelta;
	if (myUpgradeCooldowns[unitIndex] < 0.f)
	{
		myUpgradeCooldowns[unitIndex] = 0.f;
	}
}