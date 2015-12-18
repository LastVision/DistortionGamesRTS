#include "stdafx.h"
#include "ArriveBehavior.h"
#include "BlendedBehavior.h"
#include "EvadeBehavior.h"

BlendedBehavior::BlendedBehavior(const Entity& anEntity)
	: Behavior(anEntity)
	, myBehaviors(8)
{
	myBehaviors.Add(new ArriveBehavior(myEntity));
	myBehaviors.Add(new EvadeBehavior(myEntity));
}


BlendedBehavior::~BlendedBehavior()
{
	myBehaviors.DeleteAll();
}

const CU::Vector2<float>& BlendedBehavior::Update()
{
	myAcceleration.x = 0;
	myAcceleration.y = 0;

	for (int i = 0; i < myBehaviors.Size(); ++i)
	{
		myAcceleration += myBehaviors[i]->Update();
	}
	return myAcceleration;
}

void BlendedBehavior::SetTarget(const CU::Vector2<float>& aTargetPosition)
{
	myTarget = aTargetPosition;
	for (int i = 0; i < myBehaviors.Size(); ++i)
	{
		myBehaviors[i]->SetTarget(aTargetPosition);
	}
}