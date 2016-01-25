#include "stdafx.h"

#include <ArtifactMessage.h>
#include <AudioInterface.h>
#include <BuildingComponent.h>
#include "Console.h"
#include "Director.h"
#include <Entity.h>
#include <HealthComponent.h>
#include <KillUnitMessage.h>
#include <PostMaster.h>
#include <ResourceMessage.h>
#include <VictoryMessage.h>
#include <SpawnUnitMessage.h>
#include <PollingStation.h>
#include <PromotionComponent.h>
#include <SelectionComponent.h>
#include <UpgradeUnitMessage.h>
#include <XMLReader.h>

Director::Director(eOwnerType aOwnerType, const Prism::Terrain& aTerrain)
	: myOwner(aOwnerType)
	, myTerrain(aTerrain)
	, myDeadUnits(32)
	, myTimeMultiplier(1.f)
	, myVictoryPoints(0)
	, myGunpowder(60)
	, myArtifacts(0)
	, myUnitCap(0)
	, myUnitCount(0)
	, myHasUnlockedRanger(false)
	, myHasUnlockedTank(false)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_game.xml");
	tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "unitCap"), "value", myUnitCap);
	reader.CloseDocument();

	myUnits.Init(myUnitCap);
	myActiveUnits.Init(myUnitCap);

	PostMaster::GetInstance()->Subscribe(eMessageType::SPAWN_UNIT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::RESOURCE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::VICTORY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::UPGRADE_UNIT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::KILL_UNIT, this);
}

Director::~Director()
{
	myUnits.DeleteAll();
	SAFE_DELETE(myBuilding);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::SPAWN_UNIT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::RESOURCE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::VICTORY, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::UPGRADE_UNIT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::KILL_UNIT, this);
}

void Director::Update(float aDeltaTime)
{
	for (int i = 0; i < myDeadUnits.Size(); ++i)
	{
		myDeadUnits[i]->Update(aDeltaTime);
	}

	for (int i = myActiveUnits.Size()-1; i >= 0; --i)
	{
		myActiveUnits[i]->Update(aDeltaTime);

		if (myActiveUnits[i]->GetAlive() == false)
		{
			myDeadUnits.Add(myActiveUnits[i]);
			myActiveUnits[i]->RemoveSelectionRingFromScene();
			myActiveUnits.RemoveCyclicAtIndex(i);
		}
	}

	DEBUG_PRINT(myArtifacts);
	
}

void Director::RenderHealthBars(const Prism::Camera& aCamera)
{
	for (int i = 0; i < myActiveUnits.Size(); i++)
	{
		if (myActiveUnits[i]->GetAlive() == true)
		{
			myActiveUnits[i]->GetComponent<HealthComponent>()->RenderHealthBar(aCamera);
		}
		if (myActiveUnits[i]->GetAlive() == true)
		{
			myActiveUnits[i]->GetComponent<PromotionComponent>()->RenderPromotion(aCamera);
		}
	}
}

void Director::CleanUp()
{
	for (int i = myDeadUnits.Size() - 1; i >= 0; --i)
	{
		if (myDeadUnits[i]->GetShouldBeRemoved() == true)
		{
			myDeadUnits[i]->RemoveFromScene();
			myDeadUnits.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myActiveUnits.Size() - 1; i >= 0; --i)
	{
		if (myActiveUnits[i]->GetShouldBeRemoved() == true)
		{
			myActiveUnits[i]->RemoveFromScene();
			myActiveUnits.RemoveCyclicAtIndex(i);
		}
	}
}

bool Director::SpawnUnit(eUnitType aUnitType)
{
	if (aUnitType == eUnitType::RANGER && myHasUnlockedRanger == false) return false;
	if (aUnitType == eUnitType::TANK && myHasUnlockedTank == false) return false;
	if (myBuilding->GetComponent<BuildingComponent>()->IsQueueFull() == true)
	{
		return false;
	}

	if (myGunpowder >= myBuilding->GetComponent<BuildingComponent>()->GetUnitCost(aUnitType))
	{
		myGunpowder -= myBuilding->GetComponent<BuildingComponent>()->GetUnitCost(aUnitType);
		myBuilding->GetComponent<BuildingComponent>()->BuildUnit(aUnitType);
		return true;
	}
	else
	{
		if (myOwner == eOwnerType::PLAYER)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Not_Enough_Fuel", 0);
		}
	}
	return false;
}

bool Director::UpgradeUnit(eUnitType aUnitType)
{
	BuildingComponent* building = myBuilding->GetComponent<BuildingComponent>();
	if (building->IsQueueFull() == true)
	{
		return false;
	}

	if (building->CanUpgrade(aUnitType) == false)
	{
		return false;
	}

	if (myArtifacts >= building->GetUpgradeCost(aUnitType))
	{
		myArtifacts -= building->GetUpgradeCost(aUnitType);
		building->UpgradeUnit(aUnitType);
		return true;
	}

	return false;
}

void Director::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (static_cast<eOwnerType>(aMessage.myOwnerType) != myOwner) return;
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::RANGER && myHasUnlockedRanger == false) return;
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::TANK && myHasUnlockedTank == false) return;

	int unitSupplyCost = myBuilding->GetComponent<BuildingComponent>()->GetUnitSupplyCost(static_cast<eUnitType>(aMessage.myUnitType));

	if (myUnitCount + unitSupplyCost <= myUnitCap)
	{
		Entity* spawnedUnit = nullptr;

		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetUnitType() == static_cast<eUnitType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
			{
				myUnits[i]->SetState(eEntityState::IDLE);
				if (IsAlreadyActive(myUnits[i]) == true)
				{
					continue;
				}
				if (myDeadUnits.Find(myUnits[i]) > -1)
				{
					spawnedUnit = myUnits[i]; // is allowed to spawn dying unit, but not preferred
					continue;
				}
				
				spawnedUnit = myUnits[i];
				break;
			}
		}

		if (spawnedUnit != nullptr)
		{
			myUnitCount += unitSupplyCost;
			spawnedUnit->Spawn(aMessage.mySpawnPoint, aMessage.myRallyPoint);
			myActiveUnits.Add(spawnedUnit);

			int deadIndex = myDeadUnits.Find(spawnedUnit);
			if (deadIndex > -1)
			{
				myDeadUnits.RemoveCyclicAtIndex(deadIndex);
			}
			PollingStation::GetInstance()->RegisterEntity(spawnedUnit);
		}
		else
		{
			std::string msg;
			switch (myOwner)
			{
			case eOwnerType::PLAYER:
				msg = "PLAYER tried to spawn unit when at UnitCap";
				break;
			case eOwnerType::ENEMY:
				msg = "ENEMY tried to spawn unit when at UnitCap";
				break;
			case eOwnerType::NEUTRAL:
				msg = "NEUTRAL tried to spawn unit when at UnitCap";
				break;
			default:
				break;
			}

			Console::GetInstance()->AddHistory(msg, eHistoryType::WARNING);
		}
	}
}

void Director::ReceiveMessage(const ResourceMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		myGunpowder += aMessage.myResourceModifier;
		if (myGunpowder < 0)
		{
			myGunpowder = 0;
		}
	}
}

void Director::ReceiveMessage(const VictoryMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		myVictoryPoints += aMessage.myVictoryModifier;
		if (myVictoryPoints < 0)
		{
			myVictoryPoints = 0;
		}
	}
}

void Director::ReceiveMessage(const UpgradeUnitMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		const Upgrade& upgrade = aMessage.myUpgrade;
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetUnitType() == aMessage.myUnit)
			{
				HealthComponent* comp = myUnits[i]->GetComponent<HealthComponent>();
				if (comp != nullptr)
				{
					float oldArmor = comp->GetArmor();
					float newArmor = oldArmor * upgrade.myArmorModifier;
					if (int(oldArmor) == int(newArmor))
					{
						newArmor = oldArmor + 1.f;
					}

					comp->SetArmor(newArmor);
				}
			}
		}
	}
}

void Director::ReceiveMessage(const KillUnitMessage& aMessage)
{
	if (static_cast<eOwnerType>(aMessage.myOwnerType) != myOwner)
	{
		if (myHasUnlockedRanger == false && static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::RANGER)
		{
			myHasUnlockedRanger = true;
		}
		else if (myHasUnlockedTank == false && static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::TANK)
		{
			myHasUnlockedTank = true;
		}
	}
	else if (myOwner != eOwnerType::NEUTRAL)
	{
		myUnitCount -= myBuilding->GetComponent<BuildingComponent>()->GetUnitSupplyCost(static_cast<eUnitType>(aMessage.myUnitType));

		DL_ASSERT_EXP(myUnitCount >= 0, "[Director] Current unit count is negative");
	}
}

void Director::ReceiveMessage(const ArtifactMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		myArtifacts += aMessage.myArtifactModifier;
	}
}

bool Director::IsAlreadyActive(Entity* aUnit)
{
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		if (myActiveUnits[i] == aUnit)
		{
			return true;
		}
	}
	return false;
}