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

	if (closestPlayerEntity != nullptr)
	{
		myUnits[0]->GetComponent<ControllerComponent>()->Attack(closestPlayerEntity);
	}
	else
	{
		myUnits[0]->SetState(eEntityState::IDLE);
	}
}