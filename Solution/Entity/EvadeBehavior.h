#pragma once
#include "Behavior.h"

class EvadeBehavior : public Behavior
{
public:
	EvadeBehavior(const Entity& anEntity);
	~EvadeBehavior();

	const CU::Vector2<float>& Update() override;

private:
	CU::GrowingArray<Entity*> myEntitiesToEvade;
};

