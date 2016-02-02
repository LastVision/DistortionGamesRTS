#include "stdafx.h"

#include "CollisionComponent.h"
#include "EvadeBehavior.h"
#include "PollingStation.h"

#include <InputWrapper.h>
EvadeBehavior::EvadeBehavior(const Entity& anEntity)
	: Behavior(anEntity)
	, myMaxAcceleration(10000.f)
	, myEntitiesToEvade(128)
	, myPropSpeed(500.f)
	, myPropMod(50.f)
{
	//myAcceleration.x = 10.f;
}


EvadeBehavior::~EvadeBehavior()
{
}

const CU::Vector2<float>& EvadeBehavior::Update(float aDelta)
{
	myAcceleration.x = 0;
	myAcceleration.y = 0;


	myEntitiesToEvade.RemoveAll();
	//PollingStation::GetInstance()->FindAllEntitiesCloseToEntity(&myEntity, 5.f, myEntitiesToEvade);
	PollingStation::GetInstance()->FindAllUnitsCloseToEntity(&myEntity, 1.f, myEntitiesToEvade);
	EvadeEntities(aDelta, 50.f);

	myEntitiesToEvade.RemoveAll();
	PollingStation::GetInstance()->FindAllPropsCloseToEntity(&myEntity, 3.f, myEntitiesToEvade);
	EvadeEntities(aDelta, myPropSpeed);


	DEBUG_PRINT(myPropSpeed);
	DEBUG_PRINT(myPropMod);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_O))
	{
		myPropSpeed += myPropMod;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_L))
	{
		myPropSpeed -= myPropMod;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_I))
	{
		myPropMod += 10.f;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_K))
	{
		myPropMod -= 10.f;
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

void EvadeBehavior::EvadeEntities(float aDelta, float aEvadeSpeed)
{
	//float radius = myEntity.GetComponent<CollisionComponent>()->GetRadius();
	//radius2 = radius2 * radius2;

	CU::Vector2<float> forward(myEntity.GetOrientation().GetForward().x, myEntity.GetOrientation().GetForward().y);
	int test = 5;

	for (int i = 0; i < myEntitiesToEvade.Size(); ++i)
	{
		CU::Vector2<float> toTarget = myEntity.GetPosition() - myEntitiesToEvade[i]->GetPosition();

		CU::Vector2<float> normToTarget(CU::GetNormalized(toTarget));
		if (CU::Length(forward + normToTarget) < 0.5f)
		{
			float oldX = toTarget.x;
			toTarget.x = -toTarget.y;
			toTarget.y = oldX;
		}




		float distToTarget2 = CU::Length2(toTarget);
		toTarget /= (distToTarget2 + 0.00001f);
		toTarget *= aEvadeSpeed * aDelta;
		myAcceleration += toTarget;


		//CollisionComponent* collision = myEntitiesToEvade[i]->GetComponent<CollisionComponent>();
		//if (collision != nullptr)
		//{
		//	float targetRadius = collision->GetRadius();
		//	//targetRadius2 = targetRadius2 * targetRadius2;

		//	if (distToTarget2 < ((radius + targetRadius) * (radius + targetRadius)))
		//	{
		//		toTarget /= (distToTarget2 + 0.00001f);
		//		toTarget *= aEvadeSpeed * aDelta;
		//		myAcceleration += toTarget;
		//	}
		//}
	}
}