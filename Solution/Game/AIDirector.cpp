#include "stdafx.h"
#include "AIDirector.h"

AIDirector::AIDirector(const Prism::Terrain& aTerrain)
	: Director(eDirectorType::AI, aTerrain)
{
}


AIDirector::~AIDirector()
{
}

void AIDirector::Update(float aDeltaTime)
{
	Director::Update(aDeltaTime);
}
