#include "stdafx.h"
#include "Actor.h"
#include <Entity.h>

Actor::Actor(eActorType aActorType, const Prism::Terrain& aTerrain)
	: myActorType(aActorType)
	, myTerrain(aTerrain)
	, myUnits(64)
{
}


Actor::~Actor()
{
	myUnits.DeleteAll();
}

void Actor::Update(float aDeltaTime)
{
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		myUnits[i]->Update(aDeltaTime);
	}
}