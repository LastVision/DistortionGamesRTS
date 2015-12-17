#include "stdafx.h"
#include <BuildingComponent.h>
#include "Console.h"
#include "Director.h"
#include <Entity.h>
#include <HealthComponent.h>
#include <PostMaster.h>
#include <ResourceMessage.h>
#include <VictoryMessage.h>
#include <SpawnUnitMessage.h>
#include <PollingStation.h>

Director::Director(eOwnerType aOwnerType, const Prism::Terrain& aTerrain)
	: myOwner(aOwnerType)
	, myTerrain(aTerrain)
	, myUnits(64)
	, myActiveUnits(64)
	, myTimeMultiplier(1.f)
	, myVictoryPoints(0)
	, myTestGold(60)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::SPAWN_UNIT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::RESOURCE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::VICTORY, this);
}


Director::~Director()
{
	myUnits.DeleteAll();
	SAFE_DELETE(myBuilding);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::SPAWN_UNIT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::RESOURCE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::VICTORY, this);
}

void Director::Update(float aDeltaTime)
{
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		myActiveUnits[i]->Update(aDeltaTime);
	}
}

void Director::RenderHealthBars(const Prism::Camera& aCamera)
{
	for (int i = 0; i < myActiveUnits.Size(); i++)
	{
		if (myActiveUnits[i]->GetAlive() == true)
		{
			myActiveUnits[i]->GetComponent<HealthComponent>()->RenderHealthBar(aCamera);
		}
	}
}

void Director::CleanUp()
{
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
	if (myBuilding->GetComponent<BuildingComponent>()->IsQueueFull() == true)
	{
		return false;
	}

	if (myTestGold >= myBuilding->GetComponent<BuildingComponent>()->GetUnitCost(aUnitType))
	{
		myTestGold -= myBuilding->GetComponent<BuildingComponent>()->GetUnitCost(aUnitType);
		myBuilding->GetComponent<BuildingComponent>()->BuildUnit(aUnitType);
		return true;
	}
	return false;
}

void Director::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (static_cast<eOwnerType>(aMessage.myOwnerType) != myOwner) return;
	if (myActiveUnits.Size() < 64)
	{
		Entity* spawnedUnit = nullptr;

		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetUnitType() == static_cast<eUnitType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
			{
				if (IsAlreadyActive(myUnits[i]) == true)
				{
					continue;
				}
				myUnits[i]->Spawn(myBuilding->GetOrientation().GetPos() + CU::Vector3f(0.f, 0.f, -15.f));
				myActiveUnits.Add(myUnits[i]);
				spawnedUnit = myUnits[i];
				break;
			}
		}

		if (spawnedUnit != nullptr)
		{
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
		myTestGold += aMessage.myResourceModifier;
		if (myTestGold < 0)
		{
			myTestGold = 0;
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