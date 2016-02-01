#include "stdafx.h"

#include "CollisionComponent.h"
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

const CU::Vector2<float>& EvadeBehavior::Update(float aDelta)
{
	myEntitiesToEvade.RemoveAll();
	PollingStation::GetInstance()->FindAllEntitiesCloseToEntity(&myEntity, 5.f, myEntitiesToEvade);

	myAcceleration.x = 0;
	myAcceleration.y = 0;

	float radius2 = myEntity.GetComponent<CollisionComponent>()->GetRadius();
	radius2 = radius2 * radius2;

	for (int i = 0; i < myEntitiesToEvade.Size(); ++i)
	{
		CU::Vector2<float> toTarget = myEntity.GetPosition() - myEntitiesToEvade[i]->GetPosition();
		float distToTarget2 = CU::Length2(toTarget);

		CollisionComponent* collision = myEntitiesToEvade[i]->GetComponent<CollisionComponent>();
		if (collision != nullptr)
		{
			float targetRadius2 = collision->GetRadius();
			targetRadius2 = targetRadius2 * targetRadius2;

			if (distToTarget2 < radius2 + targetRadius2)
			{
				toTarget /= (distToTarget2 + 0.00001f);
				toTarget *= 1000.f * aDelta;
				myAcceleration += toTarget;
			}
		}
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