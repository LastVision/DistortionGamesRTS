#include "stdafx.h"
#include "AIDirector.h"
#include <ControllerComponent.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <PollingStation.h>

AIDirector::AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene)
	: Director(eDirectorType::AI, aTerrain)
{
	for (int i = 0; i < 1; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
		myUnits[i]->Spawn({ 20.f + i, 0.f, 40.f });
	}

	myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
		aScene, { 200.f, 0.f, 200.f }, aTerrain));
	myUnits.GetLast()->Spawn({ 100.f, 0.f, 40.f });


	myBuilding = EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::BASE_BUILING, Prism::eOctreeType::STATIC, aScene, { 130, 0, 140 }, aTerrain);
	myBuilding->AddToScene();
	myBuilding->Reset();

	/*for (int i = 0; i < myUnits.Size(); ++i)
	{
		PollingStation::GetInstance()->RegisterEntity(myUnits[i]);
	}*/

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		myActiveUnits.Add(myUnits[i]);
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits[i]);
	}
}


AIDirector::~AIDirector()
{
}

void AIDirector::Update(float aDeltaTime)
{
	Director::Update(aDeltaTime);

	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		ControllerComponent* controller = myActiveUnits[i]->GetComponent<ControllerComponent>();
		Entity* closestPlayerEntity = PollingStation::GetInstance()->FindClosestEntity(
			myActiveUnits[i]->GetOrientation().GetPos(), eOwnerType::PLAYER, controller->GetVisionRange());

		if (closestPlayerEntity != nullptr)
		{
			myActiveUnits[i]->GetComponent<ControllerComponent>()->Attack(closestPlayerEntity);
		}
		else
		{
			myActiveUnits[i]->SetState(eEntityState::IDLE);
		}
	}
}