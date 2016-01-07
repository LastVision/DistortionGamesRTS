#include "stdafx.h"
#include "AIDirector.h"
#include <BuildingComponent.h>
#include <ControllerComponent.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <GameConstants.h>
#include <PollingStation.h>
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <TimeMultiplierMessage.h>

AIDirector::AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene)
	: Director(eOwnerType::ENEMY, aTerrain)
	, myPlayerHasStarted(false)
	, myIdleUnits(32)
	, mySurviveGatherer(nullptr)
{
	for (int i = 0; i < GC::enemyGruntCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, eUnitType::GRUNT, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	for (int i = 0; i < GC::enemyRangerCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, eUnitType::RANGER, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	for (int i = 0; i < GC::enemyTankCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, eUnitType::TANK, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	PostMaster::GetInstance()->Subscribe(eMessageType::TIME_MULTIPLIER, this);
}


AIDirector::~AIDirector()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TIME_MULTIPLIER, this);
}

void AIDirector::Update(float aDeltaTime)
{
	aDeltaTime *= myTimeMultiplier;

	Director::Update(aDeltaTime);

	if (myPlayerHasStarted == false)
	{
		myPlayerHasStarted = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER).Size() > 0;
		return;
	}

	if (mySurviveGatherer != nullptr && mySurviveGatherer->GetAlive() == false)
	{
		mySurviveGatherer = nullptr;
	}

	myBuilding->Update(aDeltaTime);

	if (myBuilding->GetComponent<BuildingComponent>()->GetSpawnQueueSize() > 0)
	{
		return;
	}

	NotLoseLogic();
}

void AIDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	Director::ReceiveMessage(aMessage);
	if (aMessage.myOwnerType == static_cast<int>(eOwnerType::ENEMY))
	{
		Entity* newEntity = myActiveUnits.GetLast();
		newEntity->SetState(eEntityState::IDLE);

		myIdleUnits.Add(newEntity);
	}
}

void AIDirector::ReceiveMessage(const TimeMultiplierMessage& aMessage)
{
	if (aMessage.myOwner == eOwnerType::ENEMY)
	{
		myTimeMultiplier = aMessage.myMultiplier;
	}
}

void AIDirector::NotLoseLogic()
{
	const PollingStation* pollingStation = PollingStation::GetInstance();

	eAction action = eAction::NONE;
	CU::Vector3<float> capturePos;

	if (pollingStation->GetResourcePointsCount(eOwnerType::ENEMY) < 1 && mySurviveGatherer == nullptr)
	{
		if (myIdleUnits.Size() == 0)
		{
			action = eAction::SPAWN_GRUNT;
		}
		else
		{
			const CU::GrowingArray<Entity*> resources = pollingStation->GetResourcePoints();
			capturePos.x = resources[0]->GetPosition().x;
			capturePos.z = resources[0]->GetPosition().y;
			action = eAction::CAPTURE_POINT;
			mySurviveGatherer = myIdleUnits.GetLast();
		}
	}


	int playerVictoryPoints = pollingStation->GetVictoryPointsCount(eOwnerType::PLAYER);
	if (playerVictoryPoints > 0)
	{
		int playerCount = pollingStation->GetUnits(eOwnerType::PLAYER).Size();
		if (myIdleUnits.Size() == 0 && playerCount > myActiveUnits.Size()-1)
		{
			action = eAction::SPAWN_GRUNT;
		}
		else if (myIdleUnits.Size() > 0)
		{
			const CU::GrowingArray<Entity*> victoryPoints = pollingStation->GetVictoryPoints();
			for (int i = 0; i < victoryPoints.Size(); ++i)
			{
				if (victoryPoints[i]->GetOwner() == eOwnerType::PLAYER)
				{
					capturePos.x = victoryPoints[i]->GetPosition().x;
					capturePos.z = victoryPoints[i]->GetPosition().y;
					action = eAction::CAPTURE_POINT;
					break;
				}
			}
		}
	}

	switch (action)
	{
	case AIDirector::eAction::CAPTURE_POINT:
	{
		Entity* unit = myIdleUnits.GetLast();
		myIdleUnits.RemoveCyclicAtIndex(myIdleUnits.Size() - 1);
		unit->GetComponent<ControllerComponent>()->AttackMove(capturePos, true);
		break;
	}
	case AIDirector::eAction::SPAWN_GRUNT:
		Director::SpawnUnit(eUnitType::GRUNT);
		break;
	case AIDirector::eAction::SPAWN_RANGER:
		Director::SpawnUnit(eUnitType::RANGER);
		break;
	case AIDirector::eAction::SPAWN_TANK:
		Director::SpawnUnit(eUnitType::TANK);
		break;
	case AIDirector::eAction::NONE:
		break;
	default:
		break;
	}
}

void AIDirector::WinSlowlyLogic()
{
	const PollingStation* pollingStation = PollingStation::GetInstance();

	int victoryPoints = pollingStation->GetVictoryPointsCount(eOwnerType::ENEMY);
	int totalVictoryPoints = pollingStation->GetVictoryPoints().Size();

	if ((totalVictoryPoints / 2) > victoryPoints)
	{
		//I dont have more than 50% of VictoryPoints?!?!? CAPTURE MOAR
	}

	//If the player has more than 2 resourcepoints I need to do something about it

	//If I have less than 2 resourcepoints I need to do something about it
}