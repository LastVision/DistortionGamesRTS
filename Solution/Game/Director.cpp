#include "stdafx.h"
#include "Director.h"
#include <Entity.h>
#include <PostMaster.h>

Director::Director(eDirectorType aDirectorType, const Prism::Terrain& aTerrain)
	: myDirectorType(aDirectorType)
	, myTerrain(aTerrain)
	, myUnits(64)
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
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		myUnits[i]->Update(aDeltaTime);
	}
}

void Director::ReceiveMessage(const SpawnUnitMessage&)
{
}
