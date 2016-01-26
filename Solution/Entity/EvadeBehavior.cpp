#include "stdafx.h"
#include "EvadeBehavior.h"
#include "PollingStation.h"

EvadeBehavior::EvadeBehavior(const Entity& anEntity)
	: Behavior(anEntity)
	, myMaxAcceleration(100000.f)
	, myEntitiesToEvade(128)
{
	//myAcceleration.x = 10.f;
}


EvadeBehavior::~EvadeBehavior()
{
}

const CU::Vector2<float>& EvadeBehavior::Update()
{
	myEntitiesToEvade.RemoveAll();
	PollingStation::GetInstance()->FindAllEntitiesCloseToEntity(&myEntity, 4.f, myEntitiesToEvade);

	myAcceleration.x = 0;
	myAcceleration.y = 0;

	for (int i = 0; i < myEntitiesToEvade.Size(); ++i)
	{
		CU::Vector2<float> toTarget = myEntity.GetPosition() - myEntitiesToEvade[i]->GetPosition();
		float distToTarget2 = CU::Length2(toTarget);
		toTarget /= (distToTarget2 + 0.00001f);
		toTarget *= 3.f;
		myAcceleration += toTarget;
	}



	myAcceleration -= myEntity.GetVelocity();
	myAcceleration /= 0.1f;

	if (CU::Length2(myAcceleration) > myMaxAcceleration)
	{
		CU::Normalize(myAcceleration);
		myAcceleration *= myMaxAcceleration;
	}

	return myAcceleration * 0.75f;
}