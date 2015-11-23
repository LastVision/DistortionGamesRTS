#include "stdafx.h"

#include "AIComponent.h"
#include "BulletAIComponent.h"
#include "BulletComponent.h"
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "Constants.h"
#include "DefendMessage.h"
#include <Engine.h>
#include "Entity.h"
#include <MathHelper.h>
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "PostMaster.h"
#include <sstream>
#include <Vector.h>


BulletAIComponent::BulletAIComponent(Entity& aEntity)
	: ControllerComponent(aEntity)
	, myPhysicsComponent(nullptr)
	, myPrevEntityToFollow(nullptr)
	, myTurnRateModifier(1.f)
{
	DL_ASSERT_EXP(aEntity.GetComponent<InputComponent>() == nullptr
		, "Tried to add BulletAIComponent when there was a InputComponent");
	DL_ASSERT_EXP(aEntity.GetComponent<AIComponent>() == nullptr
		, "Tried to add BulletAIComponent when there was a AIComponent");
}

BulletAIComponent::~BulletAIComponent()
{
}

void BulletAIComponent::Init(float aSpeed, float aTurnRateModifier, CollisionManager* aCollisionManager)
{
	myEntityToFollow = nullptr;
	myCollisionManager = aCollisionManager;

	PhysicsComponent* physicsComponent = myEntity.GetComponent<PhysicsComponent>();
	if (physicsComponent != nullptr)
	{
		physicsComponent->SetVelocity(myEntity.myOrientation.GetForward() * aSpeed);
	}

	myBulletComponent = myEntity.GetComponent<BulletComponent>();

	myCanMove = true;

	myTurnRateModifier = aTurnRateModifier;
}

void BulletAIComponent::Update(float aDeltaTime)
{
	DL_ASSERT_EXP(myEntityToFollow != nullptr, "BulletAIComponent needs an entity to follow.");

	if (myEntityToFollow->GetAlive() == false && myEntity.GetAlive() == true)
	{
		myEntity.GetComponent<BulletComponent>()->SetActive(false);
		//myCollisionManager->Remove(myEntity.GetComponent<CollisionComponent>(), myEntity.GetType());
		//myEntity.Kill();
		return;
	}

	if (myPhysicsComponent == nullptr)
	{
		myPhysicsComponent = myEntity.GetComponent<PhysicsComponent>();
		DL_ASSERT_EXP(myPhysicsComponent != nullptr, "BulletAIComponent component needs physics component for movement."); // remove later
	}
	myVelocity = myPhysicsComponent->GetVelocity();

	FollowEntity(aDeltaTime);

	myPhysicsComponent->SetVelocity(myVelocity);
}

void BulletAIComponent::SetEntityToFollow(Entity* aEntity, Entity* aPlayerEntity)
{
	myEntityToFollow = aEntity;
	if (myPrevEntityToFollow == nullptr)
	{
		myPrevEntityToFollow = aPlayerEntity;
	}
}

void BulletAIComponent::FollowEntity(float aDeltaTime)
{
	myToTarget = myEntityToFollow->myOrientation.GetPos() - myEntity.myOrientation.GetPos();

	CU::Normalize(myToTarget);

	myVelocity += myToTarget * (myBulletComponent->GetCurrentLifeTime() * myTurnRateModifier * aDeltaTime);

	CU::Normalize(myVelocity);

	CU::Vector3<float> up(myEntity.myOrientation.GetUp());

	up = up + myToTarget * aDeltaTime;

	CU::Normalize(up);

	CU::Vector3<float> right = CU::Cross(up, myVelocity);
	up = CU::Cross(myVelocity, right);

	right = CU::Cross(up, myVelocity);

	myEntity.myOrientation.myMatrix[0] = right.x;
	myEntity.myOrientation.myMatrix[1] = right.y;
	myEntity.myOrientation.myMatrix[2] = right.z;
	myEntity.myOrientation.myMatrix[3] = 0;

	myEntity.myOrientation.myMatrix[4] = up.x;
	myEntity.myOrientation.myMatrix[5] = up.y;
	myEntity.myOrientation.myMatrix[6] = up.z;
	myEntity.myOrientation.myMatrix[7] = 0;

	myEntity.myOrientation.myMatrix[8] = myVelocity.x;
	myEntity.myOrientation.myMatrix[9] = myVelocity.y;
	myEntity.myOrientation.myMatrix[10] = myVelocity.z;
	myEntity.myOrientation.myMatrix[11] = 0;

	myVelocity *= myPhysicsComponent->GetSpeed();
}