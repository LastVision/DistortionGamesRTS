#include "stdafx.h"
#include "AIActor.h"

AIActor::AIActor(const Prism::Terrain& aTerrain)
	: Actor(eActorType::AI, aTerrain)
{
}


AIActor::~AIActor()
{
}

void AIActor::Update(float aDeltaTime)
{
	Actor::Update(aDeltaTime);
}
