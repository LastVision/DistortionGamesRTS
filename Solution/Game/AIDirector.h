#pragma once
#include "Director.h"

class AIDirector : public Director
{
public:
	AIDirector(const Prism::Terrain& aTerrain);
	~AIDirector();

	void Update(float aDeltaTime) override;
};

