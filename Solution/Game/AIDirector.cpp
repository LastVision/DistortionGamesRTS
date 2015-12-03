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
	}

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

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		ControllerComponent* controller = myUnits[i]->GetComponent<ControllerComponent>();
		Entity* closestPlayerEntity = PollingStation::GetInstance()->FindClosestEntity(
			myUnits[i]->GetOrientation().GetPos(), eOwnerType::PLAYER, controller->GetVisionRange());

		if (closestPlayerEntity != nullptr)
		{
			myUnits[i]->GetComponent<ControllerComponent>()->Attack(closestPlayerEntity);
		}
		else
		{
			myUnits[i]->SetState(eEntityState::IDLE);
		}
	}
}