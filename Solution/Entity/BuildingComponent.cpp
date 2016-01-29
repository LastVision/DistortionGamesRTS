#include "stdafx.h"
#include "BuildingComponent.h"
#include "CollisionComponent.h"
#include "GrenadeComponent.h"
#include <Intersection.h>
#include <PostMaster.h>
#include "PollingStation.h"
#include <SpawnUnitMessage.h>
#include <UpgradeUnitMessage.h>

BuildingComponent::BuildingComponent(Entity& aEntity, BuildingComponentData& aData)
	: Component(aEntity)
	, myMaxBuildTime(2.f)
	, myUnitCosts(aData.myUnitCosts)
	, myUnitBuildTimes(aData.myUnitBuildTimes)
	, myUnitUpgrades(aData.myUnitUpgrades)
	, myUnitSupplyCosts(aData.myUnitSupplyCosts)
	, myMaxQueue(aData.myMaxQueue)
	, myMineFieldRadius(aData.myMineFieldRadius)
	, myUnitsInMineField(8)
	, myUpgradesInQueue(aData.myUpgradesInQueue)
{
	for (int i = 0; i < 3; ++i)
	{
		myUnitUpgradeProgress[i] = 0;

		myUpgradeCooldowns[i] = myUnitUpgrades[i][0].myCooldown;
		myUpgradeMaxCooldowns[i] = myUpgradeCooldowns[i];
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

	CU::Vector2<float> forward(myEntity.GetOrientation().GetForward().x, myEntity.GetOrientation().GetForward().z);
	mySpawnPoint = myEntity.GetPosition() + (-forward * 14.f);
	myRallyPoint = myEntity.GetPosition() + (-forward * 15.f);
}

void BuildingComponent::Update(float aDeltaTime)
{
	HandleMineField();
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
				myUpgradeMaxCooldowns[unitIndex] = myUpgradeCooldowns[unitIndex];
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
	if (myBuildQueue.size() < myMaxQueue)
	{
		myBuildQueue.push({ aUnitType, false });
	}
}

void BuildingComponent::UpgradeUnit(eUnitType aUnitType)
{
	if (myUpgradesInQueue == true &&myBuildQueue.size() < myMaxQueue)
	{
		myBuildQueue.push({ aUnitType, true });
		int unitIndex = static_cast<int>(aUnitType);
		int upgradeIndex = myUnitUpgradeProgress[unitIndex];
		myUnitUpgrades[unitIndex][upgradeIndex].myInProgress = true;
	}
	else if (myUpgradesInQueue == false)
	{
		int unitIndex = static_cast<int>(aUnitType);
		int upgradeIndex = myUnitUpgradeProgress[unitIndex];

		PostMaster::GetInstance()->SendMessage(UpgradeUnitMessage(aUnitType, myUnitUpgrades[unitIndex][upgradeIndex]
			, myEntity.GetOwner()));
		++myUnitUpgradeProgress[unitIndex];
		upgradeIndex = myUnitUpgradeProgress[unitIndex];
		if (upgradeIndex < 3)
		{
			myUpgradeCooldowns[unitIndex] = myUnitUpgrades[unitIndex][upgradeIndex].myCooldown;
			myUpgradeMaxCooldowns[unitIndex] = myUpgradeCooldowns[unitIndex];
		}
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
	
	if (myIgnoreBuildTime == true)
	{
		myUpgradeCooldowns[unitIndex] = 0.f;
		return;
	}

	myUpgradeCooldowns[unitIndex] -= aDelta;
	if (myUpgradeCooldowns[unitIndex] < 0.f)
	{
		myUpgradeCooldowns[unitIndex] = 0.f;
	}
}

void BuildingComponent::HandleMineField()
{
	CheckUnitsForRemove(myUnitsInMineField);
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myUnitsInMineField);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myUnitsInMineField);
	}
	else if (myEntity.GetOwner() == eOwnerType::ENEMY)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myUnitsInMineField);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myUnitsInMineField);
	}
	
	if (myUnitsInMineField.Size() > 0)
	{
		Entity* toThrowAt = myUnitsInMineField[rand() % myUnitsInMineField.Size()];
		CU::Vector3<float> throwPos(toThrowAt->GetPosition().x, 1.f, toThrowAt->GetPosition().y);
		myEntity.GetComponent<GrenadeComponent>()->ThrowGrenade(throwPos);
	}

}

void BuildingComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
{
	for (int i = someUnits.Size() - 1; i >= 0; --i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myMineFieldRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()) == false
			|| (someUnits[i]->GetState() == eEntityState::DIE && someUnits[i]->GetAlive() == false))
		{
			someUnits.RemoveCyclicAtIndex(i);
		}
	}
}

void BuildingComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits
	, CU::GrowingArray<Entity*>& someUnitsOut) const
{
	for (int i = 0; i < someUnits.Size(); ++i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myMineFieldRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()))
		{
			if (someUnitsOut.Find(current) < 0)
			{
				someUnitsOut.Add(current);
			}
		}
	}
}