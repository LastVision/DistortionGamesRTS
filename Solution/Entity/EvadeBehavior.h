#pragma once
#include "Behavior.h"

class EvadeBehavior : public Behavior
{
public:
	EvadeBehavior(const Entity& anEntity);
	~EvadeBehavior();

	const CU::Vector2<float>& Update(float aDelta) override;

private:
	float myMaxAcceleration;
	CU::GrowingArray<Entity*> myEntitiesToEvade;
};

