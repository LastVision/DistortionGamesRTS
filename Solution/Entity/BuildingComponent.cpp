#include "stdafx.h"
#include "BuildingComponent.h"
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <UpgradeUnitMessage.h>

BuildingComponent::BuildingComponent(Entity& aEntity, BuildingComponentData& aData)
	: Component(aEntity)
	, myMaxBuildTime(2.f)
	, myUnitCosts(aData.myUnitCosts)
	, myUnitBuildTimes(aData.myUnitBuildTimes)
	, myUnitUpgrades(aData.myUnitUpgrades)
	, myUnitSupplyCosts(aData.myUnitSupplyCosts)
{
	for (int i = 0; i < 3; ++i)
	{
		myUnitUpgradeProgress[i] = 0;

		myUpgradeCooldowns[i] = myUnitUpgrades[i][0].myCooldown;
		myUpgradeMaxCooldowns[i] = myUnitUpgrades[i][0].myCooldown;
	}
}

BuildingComponent::~BuildingComponent()
{
}

void BuildingComponent::Reset()
{
	myCurrentBuildTime = 0.f;
	myIgnoreBuildTime = false;
	myEntity.myAlive = true;
	mySpawnPoint = myEntity.GetPosition() + CU::Vector2<float>(0, -15.f);
	myRallyPoint = myEntity.GetPosition() + CU::Vector2<float>(1.f, -16.5f);
}

void BuildingComponent::Update(float aDeltaTime)
{
	UpdateUpgradeCooldown(aDeltaTime, eUnitType::GRUNT);
	UpdateUpgradeCooldown(aDeltaTime, eUnitType::RANGER);
	UpdateUpgradeCooldown(aDeltaTime, eUnitType::TANK);

	if (myBuildQueue.empty() == true)
	{
		return;
	}

	const BuildInfo& currentOrder = myBuildQueue.front();
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
			if (upgradeIndex < 3)
			{
				myUpgradeCooldowns[unitIndex] = myUnitUpgrades[unitIndex][upgradeIndex].myCooldown;
			}
		}
		else
		{
			PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(currentOrder.myUnit, myEntity.GetOwner(), mySpawnPoint, myRallyPoint));
		}

		myBuildQueue.pop();
		myCurrentBuildTime = 0.f;
	}
}

void BuildingComponent::BuildUnit(eUnitType aUnitType)
{
	if (myBuildQueue.size() < BUILD_QUEUE_SIZE)
	{
		myBuildQueue.push({ aUnitType, false });
	}
}

void BuildingComponent::UpgradeUnit(eUnitType aUnitType)
{
	if (myBuildQueue.size() < BUILD_QUEUE_SIZE)
	{
		myBuildQueue.push({ aUnitType, true });
		int unitIndex = static_cast<int>(aUnitType);
		int upgradeIndex = myUnitUpgradeProgress[unitIndex];
		myUnitUpgrades[unitIndex][upgradeIndex].myInProgress = true;
	}
}

const int& BuildingComponent::GetUnitCost(eUnitType aUnitType) const
{
	return myUnitCosts[static_cast<int>(aUnitType)];
}

const int& BuildingComponent::GetUnitSupplyCost(eUnitType aUnitType) const
{
	return myUnitSupplyCosts[static_cast<int>(aUnitType)];
}

int BuildingComponent::GetUpgradeCost(eUnitType aUnitType)
{
	int unitIndex = static_cast<int>(aUnitType);
	int upgradeIndex = myUnitUpgradeProgress[unitIndex];
	return myUnitUpgrades[unitIndex][upgradeIndex].myCost;
}

const int& BuildingComponent::GetUpgradeCost(eUnitType aUnitType, int aUpgradeLevel) const
{
	return myUnitUpgrades[static_cast<int>(aUnitType)][aUpgradeLevel].myCost;
}

bool BuildingComponent::CanUpgrade(eUnitType aUnitType) const
{
	int unitIndex = static_cast<int>(aUnitType);
	int upgradeIndex = myUnitUpgradeProgress[unitIndex];

	if (upgradeIndex < 3)
	{
		if (myUnitUpgrades[unitIndex][upgradeIndex].myInProgress == true)
		{
			return false;
		}

		if (myUpgradeCooldowns[unitIndex] > 0.f)
		{
			return false;
		}
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