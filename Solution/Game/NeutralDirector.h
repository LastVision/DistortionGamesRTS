#pragma once
#include "Director.h"
class NeutralDirector : public Director
{
public:
	NeutralDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene);
	~NeutralDirector();

	void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const TimeMultiplierMessage& aMessage) override;
};

