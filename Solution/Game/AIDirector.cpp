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
	, mySpawnTimer(0.f)
	, myOptimalAttackerCount(0)
	, myOptimalGathererCount(1)
	, myCurrentAttackerCount(0)
	, myCurrentGathererCount(0)
	, myPointOneGatherers(8)
	, myPointTwoGatherers(8)
	, myAttackers(8)
	, myUnitQueue(8)
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


	for (int i = 0; i < 2; ++i)
	{
		myActiveUnits.Add(myUnits[i]);
		myActiveUnits[i]->Spawn({ 140.f, 0.f, 130.f });
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits[i]);
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

	//debug only, remove when handled by GUI //Linus
	int enemyVictoryPoints = myVictoryPoints;
	DEBUG_PRINT(enemyVictoryPoints);
	//debug end

	Director::Update(aDeltaTime);

	CleanUpGatherers();
	CleanUpAttackers();


	mySpawnTimer += aDeltaTime;
	myBuilding->Update(aDeltaTime);

	int playerUnitCount = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER).Size();
	myOptimalAttackerCount = playerUnitCount;


	if (myCurrentAttackerCount < myOptimalAttackerCount)
	{
		//myBuilding->GetComponent<BuildingComponent>()->BuildUnit(eUnitType::RANGER);
		if (Director::SpawnUnit(eUnitType::RANGER) == true)
		{
			myUnitQueue.Add(eUnitActionType::ATTACKER);
			++myCurrentAttackerCount;
		}
	}

	if (myCurrentGathererCount < myOptimalGathererCount)
	{
		//myBuilding->GetComponent<BuildingComponent>()->BuildUnit(eUnitType::GRUNT);
		if (Director::SpawnUnit(eUnitType::GRUNT) == true)
		{
			myUnitQueue.Add(eUnitActionType::GATHERER);
			++myCurrentGathererCount;
		}
	}
}

void AIDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	Director::ReceiveMessage(aMessage);
	if (aMessage.myOwnerType == static_cast<int>(eOwnerType::ENEMY))
	{
		Entity* newEntity = myActiveUnits.GetLast();
		newEntity->SetState(eEntityState::IDLE);

		if (myUnitQueue[0] == eUnitActionType::GATHERER)
		{
			ActivateGatherer(newEntity);
		}
		else if (myUnitQueue[0] == eUnitActionType::ATTACKER)
		{
			ActivateAttacker(newEntity);
		}

		myUnitQueue.RemoveNonCyclicAtIndex(0);
	}
	//if (aMessage.myOwnerType != static_cast<int>(eOwnerType::ENEMY)) return;
	//if (myActiveUnits.Size() < 64)
	//{
	//	for (int i = 0; i < myUnits.Size(); ++i)
	//	{
	//		if (myUnits[i]->GetUnitType() == static_cast<eUnitType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
	//		{
	//			if (IsAlreadyActive(myUnits[i]) == true)
	//			{
	//				continue;
	//			}
	//			myUnits[i]->Spawn(myBuilding->GetOrientation().GetPos() + CU::Vector3f(0.f, 0.f, -15.f));
	//			myActiveUnits.Add(myUnits[i]);
	//			break;
	//		}
	//	}
	//	PollingStation::GetInstance()->RegisterEntity(myActiveUnits.GetLast());


	//	Entity* newEntity = myActiveUnits.GetLast();
	//	newEntity->SetState(eEntityState::IDLE);

	//	if (myUnitQueue[0] == eUnitActionType::GATHERER)
	//	{
	//		ActivateGatherer(newEntity);
	//	}
	//	else if (myUnitQueue[0] == eUnitActionType::ATTACKER)
	//	{
	//		ActivateAttacker(newEntity);
	//	}
	//	
	//	myUnitQueue.RemoveNonCyclicAtIndex(0);
	//}
}

void AIDirector::ReceiveMessage(const TimeMultiplierMessage& aMessage)
{
	if (aMessage.myOwner == eOwnerType::ENEMY)
	{
		myTimeMultiplier = aMessage.myMultiplier;
	}
}

void AIDirector::CleanUpGatherers()
{
	//If we have enough resources, remove some units from gathering and make available
	//to the other "Advisors"
}

void AIDirector::CleanUpAttackers()
{
	for (int i = myAttackers.Size() - 1; i >= 0; --i)
	{
		if (myAttackers[i]->GetAlive() == false)
		{
			myAttackers.RemoveCyclicAtIndex(i);
			--myCurrentAttackerCount;
		}
	}
}

void AIDirector::ActivateGatherer(Entity* aEntity)
{
	const CU::GrowingArray<Entity*>& resourcePoints = PollingStation::GetInstance()->GetResourcePoints();
	int pointIndex = -1;
	if (myPointOneGatherers.Size() < myPointTwoGatherers.Size())
	{
		pointIndex = 0;
		myPointOneGatherers.Add(aEntity);
	}
	else
	{
		pointIndex = 1;
		myPointTwoGatherers.Add(aEntity);
	}


	CU::Vector2<float> pos = resourcePoints[pointIndex]->GetPosition();
	aEntity->GetComponent<ControllerComponent>()->MoveTo({ pos.x, 0.f, pos.y }, true);
}

void AIDirector::ActivateAttacker(Entity* aEntity)
{
	CU::Vector3<float> position = aEntity->GetOrientation().GetPos();

	Entity* closestPlayerUnit = PollingStation::GetInstance()->FindClosestEntity(position, eOwnerType::PLAYER);

	aEntity->GetComponent<ControllerComponent>()->AttackTarget(closestPlayerUnit, true);
	myAttackers.Add(aEntity);
}