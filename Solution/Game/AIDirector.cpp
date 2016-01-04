#include "stdafx.h"
#include "AIDirector.h"
#include <BuildingComponent.h>
#include <ControllerComponent.h>
#include <TimeMultiplierMessage.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <PollingStation.h>
#include <PostMaster.h>
#include <SpawnUnitMessage.h>

AIDirector::AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene)
	: Director(eOwnerType::ENEMY, aTerrain)
	, myPlayerHasStarted(false)
	, myIdleUnits(32)
	, mySurviveGatherer(nullptr)
{
	for (int i = 0; i < 64; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, eUnitType::GRUNT, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, eUnitType::RANGER, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
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

	if (pollingStation->GetResourcePointsCount(eOwnerType::ENEMY) < 1 && mySurviveGatherer == nullptr)
	{
		if (myIdleUnits.Size() == 0)
		{
			Director::SpawnUnit(eUnitType::GRUNT);
		}
		else
		{
			mySurviveGatherer = myIdleUnits.GetLast();
			myIdleUnits.RemoveCyclicAtIndex(myIdleUnits.Size() - 1);

			const CU::GrowingArray<Entity*> resources = pollingStation->GetResourcePoints();
			CU::Vector3<float> pos(resources[0]->GetPosition().x, 0.f
				, resources[0]->GetPosition().y);
			mySurviveGatherer->GetComponent<ControllerComponent>()->MoveTo(pos, true);
		}
	}


	int playerVictoryPoints = pollingStation->GetVictoryPointsCount(eOwnerType::PLAYER);
	if (playerVictoryPoints > 0)
	{
		int playerCount = pollingStation->GetUnits(eOwnerType::PLAYER).Size();
		if (myIdleUnits.Size() == 0 && playerCount > myActiveUnits.Size()-1)
		{
			Director::SpawnUnit(eUnitType::GRUNT);
		}
		else if (myIdleUnits.Size() > 0)
		{
			const CU::GrowingArray<Entity*> victoryPoints = pollingStation->GetVictoryPoints();
			for (int i = 0; i < victoryPoints.Size(); ++i)
			{
				if (victoryPoints[i]->GetOwner() == eOwnerType::PLAYER)
				{
					Entity* attacker = myIdleUnits.GetLast();
					myIdleUnits.RemoveCyclicAtIndex(myIdleUnits.Size() - 1);

					CU::Vector3<float> pos(victoryPoints[i]->GetPosition().x, 0.f
						, victoryPoints[i]->GetPosition().y);
					attacker->GetComponent<ControllerComponent>()->MoveTo(pos, true);
					break;
				}
			}
		}
	}
}