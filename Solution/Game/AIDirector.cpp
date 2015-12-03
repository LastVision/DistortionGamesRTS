#include "stdafx.h"
#include "AIDirector.h"
#include <ControllerComponent.h>
#include <Entity.h>
#include <EntityFactory.h>
#include "PollingStation.h"

AIDirector::AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene)
	: Director(eDirectorType::AI, aTerrain)
{
	for (int i = 0; i < 1; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}
	myBuilding = EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::BASE_BUILING, Prism::eOctreeType::STATIC, aScene, { 130, 0, 140 }, aTerrain);
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		PollingStation::GetInstance()->RegisterEntity(myUnits[i]);
	}
}


AIDirector::~AIDirector()
{
}

void AIDirector::Update(float aDeltaTime)
{
	Director::Update(aDeltaTime);

	Entity* closestPlayerEntity = PollingStation::GetInstance()->FindClosestEntity(
		myUnits[0]->GetOrientation().GetPos(), eOwnerType::PLAYER);

	//myUnits[0]->GetComponent<ControllerComponent>()->MoveTo(closestPlayerEntity->GetOrientation().GetPos(), true);
	myUnits[0]->GetComponent<ControllerComponent>()->Attack(closestPlayerEntity);
}