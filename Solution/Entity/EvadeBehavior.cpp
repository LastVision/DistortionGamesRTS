#include "stdafx.h"
#include "EvadeBehavior.h"


EvadeBehavior::EvadeBehavior(const Entity& anEntity)
	: Behavior(anEntity)
{
	//myAcceleration.x = 10.f;
}


EvadeBehavior::~EvadeBehavior()
{
}

const CU::Vector2<float>& EvadeBehavior::Update()
{
	return myAcceleration;
}