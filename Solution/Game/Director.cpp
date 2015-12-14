#include "stdafx.h"
#include "Director.h"
#include <Entity.h>
#include <PostMaster.h>

Director::Director(eDirectorType aDirectorType, const Prism::Terrain& aTerrain)
	: myDirectorType(aDirectorType)
	, myTerrain(aTerrain)
	, myUnits(64)
	, myActiveUnits(64)
	, myTimeMultiplier(1.f)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::SPAWN_UNIT, this);
}


Director::~Director()
{
	myUnits.DeleteAll();
	SAFE_DELETE(myBuilding);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::SPAWN_UNIT, this);
}

void Director::Update(float aDeltaTime)
{
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		myActiveUnits[i]->Update(aDeltaTime);
	}
}

void Director::CleanUp()
{
	for (int i = myActiveUnits.Size() - 1; i >= 0; --i)
	{
		if (myActiveUnits[i]->GetShouldBeRemoved() == true)
		{
			myActiveUnits.RemoveCyclicAtIndex(i);
		}
	}
}

void Director::ReceiveMessage(const SpawnUnitMessage&)
{
}