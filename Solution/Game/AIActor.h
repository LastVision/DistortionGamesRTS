#pragma once
#include "Actor.h"
class AIActor : public Actor
{
public:
	AIActor(const Prism::Terrain& aTerrain);
	~AIActor();

	void Update(float aDeltaTime) override;
};

