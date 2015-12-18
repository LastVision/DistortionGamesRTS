#pragma once
#include "Behavior.h"

class BlendedBehavior : public Behavior
{
public:
	BlendedBehavior(const Entity& anEntity);
	~BlendedBehavior();

	const CU::Vector2<float>& Update() override;
	void SetTarget(const CU::Vector2<float>& aTargetPosition) override;

private:
	CU::GrowingArray<Behavior*> myBehaviors;
};

