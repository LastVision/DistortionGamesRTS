#include "stdafx.h"
#include "Director.h"
#include <Entity.h>

Director::Director(eDirectorType aDirectorType, const Prism::Terrain& aTerrain)
	: myDirectorType(aDirectorType)
	, myTerrain(aTerrain)
	, myUnits(64)
{
}


Director::~Director()
{
	myUnits.DeleteAll();
}

void Director::Update(float aDeltaTime)
{
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		myUnits[i]->Update(aDeltaTime);
	}
}

void Director::CleanUp()
{
	for (int i = myUnits.Size()-1; i >= 0; --i)
	{
		if (myUnits[i]->GetAlive() == false)
		{
			myUnits.DeleteCyclicAtIndex(i);
		}
	}
}