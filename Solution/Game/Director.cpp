#include "stdafx.h"
#include "Director.h"
#include <Entity.h>
#include <PostMaster.h>
#include <ResourceMessage.h>
#include <VictoryMessage.h>

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

void Director::ReceiveMessage(const SpawnUnitMessage&)
{
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