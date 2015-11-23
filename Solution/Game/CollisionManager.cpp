#include "stdafx.h"

#include "BulletComponent.h"
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "CollisionNote.h"
#include "Entity.h"
#include "Enums.h"
#include <Intersection.h>
#include <sstream>

#include "AIComponent.h"
#include "HealthComponent.h"
#include "PostMaster.h"
#include "PowerUpMessage.h"
#include "SpawnExplosionMessage.h"

CollisionManager::CollisionManager()
	: myPlayers(2)
	, myAllies(64)
	, myEnemies(64)
	, myPlayerBullets(128)
	, myEnemyBullets(128)
	, myTriggers(16)
	, myProps(4096)
	, myPowerUps(16)
	, myDefendables(16)
	, myStructures(16)
	, myPlayerFilter(0)
	, myAllyFilter(0)
	, myEnemyFilter(0)
	, myPlayerBulletFilter(0)
	, myEnemyBulletFilter(0)
	, myTriggerFilter(0)
	, myPropFilter(0)
	, myPowerUpFilter(0)
	, myDefendableFilter(0)
	, myStructureFilter(0)
{
	//myPlayerFilter = eEntityType::ENEMY | eEntityType::ENEMY_BULLET | eEntityType::TRIGGER;
	myAllyFilter = 0;
	myPlayerBulletFilter = eEntityType::ENEMY | eEntityType::PROP | eEntityType::STRUCTURE;
	myEnemyBulletFilter = eEntityType::PLAYER | eEntityType::DEFENDABLE;
	myTriggerFilter = eEntityType::PLAYER;
	myPowerUpFilter = eEntityType::PLAYER;
	myPropFilter = eEntityType::PLAYER;
	myDefendableFilter = eEntityType::PLAYER;
	myStructureFilter = eEntityType::PLAYER;
	//myEnemyFilter = eEntityType::PLAYER;
	PostMaster::GetInstance()->Subscribe(eMessageType::POWER_UP, this);
}

CollisionManager::~CollisionManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::POWER_UP, this);
}

void CollisionManager::Add(CollisionComponent* aComponent, eEntityType aEnum)
{
	switch (aEnum)
	{
	case eEntityType::PLAYER:
#ifdef _DEBUG
		DL_ASSERT_EXP(myPlayers.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myPlayers.Add(aComponent);
		break;
	case eEntityType::ENEMY:
#ifdef _DEBUG
		DL_ASSERT_EXP(myEnemies.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myEnemies.Add(aComponent);
		break;
	case eEntityType::PLAYER_BULLET:
#ifdef _DEBUG
		DL_ASSERT_EXP(myPlayerBullets.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myPlayerBullets.Add(aComponent);
		break;
	case eEntityType::ENEMY_BULLET:
#ifdef _DEBUG
		DL_ASSERT_EXP(myEnemyBullets.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myEnemyBullets.Add(aComponent);
		break;
	case eEntityType::TRIGGER:
#ifdef _DEBUG
		DL_ASSERT_EXP(myTriggers.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myTriggers.Add(aComponent);
		break;
	case eEntityType::POWERUP:
#ifdef _DEBUG
		DL_ASSERT_EXP(myPowerUps.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myPowerUps.Add(aComponent);
		break;
	case eEntityType::PROP:
#ifdef _DEBUG
		DL_ASSERT_EXP(myProps.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myProps.Add(aComponent);
		break;
	case eEntityType::DEFENDABLE:
#ifdef _DEBUG
		DL_ASSERT_EXP(myDefendables.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myDefendables.Add(aComponent);
		break;
	case eEntityType::STRUCTURE:
#ifdef _DEBUG
		DL_ASSERT_EXP(myStructures.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myStructures.Add(aComponent);
		break;
	case eEntityType::ALLY:
#ifdef _DEBUG
		DL_ASSERT_EXP(myAllies.Find(aComponent) < 0, "component already in collisionmanager.");
#endif
		myAllies.Add(aComponent);
		break;
	default:
		DL_ASSERT("Tried to Add invalid EntityType to CollisionManager.");
		break;
	}
}


void CollisionManager::Remove(CollisionComponent* aComponent, eEntityType aEnum)
{
	switch (aEnum)
	{
	case eEntityType::PLAYER:
		myPlayers.RemoveCyclic(aComponent);
		break;
	case eEntityType::ENEMY:
		myEnemies.RemoveCyclic(aComponent);
		break;
	case eEntityType::PLAYER_BULLET:
		myPlayerBullets.RemoveCyclic(aComponent);
		break;
	case eEntityType::ENEMY_BULLET:
		myEnemyBullets.RemoveCyclic(aComponent);
		break;
	case eEntityType::TRIGGER:
		myTriggers.RemoveCyclic(aComponent);
		break;
	case eEntityType::POWERUP:
		myPowerUps.RemoveCyclic(aComponent);
		break;
	case eEntityType::PROP:
		myProps.RemoveCyclic(aComponent);
		break;
	case eEntityType::DEFENDABLE:
		myDefendables.RemoveCyclic(aComponent);
		break;
	case eEntityType::STRUCTURE:
		myStructures.RemoveCyclic(aComponent);
		break;
	case eEntityType::ALLY:
		myAllies.RemoveCyclic(aComponent);
		break;
	default:
		DL_ASSERT("Tried to Remove invalid EntityType to CollisionManager.");
		break;
	}
}

void CollisionManager::Update()
{
	for (int i = myPlayers.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myPlayers[i], myPlayerFilter);
	}
	for (int i = myEnemies.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myEnemies[i], myEnemyFilter);
	}
	for (int i = myPlayerBullets.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myPlayerBullets[i], myPlayerBulletFilter);
	}
	for (int i = myEnemyBullets.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myEnemyBullets[i], myEnemyBulletFilter);
	}
	for (int i = myTriggers.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myTriggers[i], myTriggerFilter);
	}
	for (int i = myPowerUps.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myPowerUps[i], myPowerUpFilter);
	}
	for (int i = myProps.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myProps[i], myPropFilter);
	}
	for (int i = myDefendables.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myDefendables[i], myDefendableFilter);
	}
	for (int i = myStructures.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myStructures[i], myStructureFilter);
	}
	for (int i = myAllies.Size() - 1; i >= 0; --i)
	{
		CheckAllCollisions(myAllies[i], myStructureFilter);
	}
}

void CollisionManager::CleanUp()
{
	for (int i = myEnemies.Size() - 1; i >= 0; --i)
	{
		if (myEnemies[i]->GetEntity().GetAlive() == false)
		{
			myEnemies.RemoveCyclicAtIndex(i);
		}
	}
	for (int i = myTriggers.Size() - 1; i >= 0; --i)
	{
		if (myTriggers[i]->GetEntity().GetAlive() == false)
		{
			myTriggers.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myProps.Size() - 1; i >= 0; --i)
	{
		if (myProps[i]->GetEntity().GetAlive() == false)
		{
			myProps.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myPowerUps.Size() - 1; i >= 0; --i)
	{
		if (myPowerUps[i]->GetEntity().GetAlive() == false)
		{
			myPowerUps.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myDefendables.Size() - 1; i >= 0; --i)
	{
		if (myDefendables[i]->GetEntity().GetAlive() == false)
		{
			myDefendables.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myStructures.Size() - 1; i >= 0; --i)
	{
		if (myStructures[i]->GetEntity().GetAlive() == false)
		{
			myStructures.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myAllies.Size() - 1; i >= 0; --i)
	{
		if (myAllies[i]->GetEntity().GetAlive() == false)
		{
			myAllies.RemoveCyclicAtIndex(i);
		}
	}
}

void CollisionManager::ReceiveMessage(const PowerUpMessage& aMessage)
{
	DisableEnemiesWithinSphere(aMessage.GetPosition(), aMessage.GetRadius(), aMessage.GetTime());
}

void CollisionManager::CheckAllCollisions(CollisionComponent* aComponent, int aFilter)
{
	bool collided = false;

	if (collided == false && aFilter & eEntityType::PLAYER)
	{
		collided = CheckCollision(aComponent, myPlayers);
	}
	if (collided == false && aFilter & eEntityType::ENEMY)
	{
		collided = CheckCollision(aComponent, myEnemies);
	}
	if (collided == false && aFilter & eEntityType::PLAYER_BULLET)
	{
		collided = CheckCollision(aComponent, myPlayerBullets);
	}
	if (collided == false && aFilter & eEntityType::ENEMY_BULLET)
	{
		collided = CheckCollision(aComponent, myEnemyBullets);
	}
	if (collided == false && aFilter & eEntityType::TRIGGER)
	{
		collided = CheckCollision(aComponent, myTriggers);
	}
	if (collided == false && aFilter & eEntityType::PROP)
	{
		collided = CheckCollision(aComponent, myProps);
	}
	if (collided == false && aFilter & eEntityType::POWERUP)
	{
		collided = CheckCollision(aComponent, myPowerUps);
	}
	if (collided == false && aFilter & eEntityType::DEFENDABLE)
	{
		collided = CheckCollision(aComponent, myDefendables);
	}
	if (collided == false && aFilter & eEntityType::STRUCTURE)
	{
		collided = CheckCollision(aComponent, myStructures);
	}
	if (collided == false && aFilter & eEntityType::ALLY)
	{
		collided = CheckCollision(aComponent, myAllies);
	}
}

bool CollisionManager::CheckCollision(CollisionComponent* aComponent
	, CU::GrowingArray<CollisionComponent*>& someOtherComponents)
{
	for (int i = someOtherComponents.Size() - 1; i >= 0; --i)
	{
		if (aComponent->CheckCollision(someOtherComponents[i]) == true)
		{
			Entity& theOther = someOtherComponents[i]->GetEntity();
			aComponent->GetEntity().SendNote(CollisionNote(theOther, *this));
			return true;
		}
	}
	return false;
}

void CollisionManager::DisableEnemiesWithinSphere(CU::Vector3<float> aCenter, float aRadius, float aTime)
{
	Sphere sphere;
	sphere.myCenterPosition = aCenter;
	sphere.myRadius = aRadius;
	sphere.myRadiusSquared = aRadius * aRadius;

	for (int i = myEnemies.Size() - 1; i >= 0; --i)
	{
		if (CU::Intersection::SphereVsSphere(sphere, myEnemies[i]->GetSphere()) == true)
		{
			myEnemies[i]->GetEntity().GetComponent<AIComponent>()->DisableMovement(aTime);
		}
	}
}

void CollisionManager::DamageEnemiesWithinSphere(CU::Vector3<float> aCenter, float aRadius, int aDamage)
{
	Sphere sphere;
	sphere.myCenterPosition = aCenter;
	sphere.myRadius = aRadius;
	sphere.myRadiusSquared = aRadius * aRadius;

	for (int i = myEnemies.Size() - 1; i >= 0; --i)
	{
		if (CU::Intersection::SphereVsSphere(sphere, myEnemies[i]->GetSphere()) == true && myEnemies[i]->GetEntity().GetComponent<HealthComponent>()->IsAlive() == true)
		{
			myEnemies[i]->GetEntity().GetComponent<HealthComponent>()->RemoveHealth(aDamage);
		}
	}

	for (int i = myProps.Size() - 1; i >= 0; --i)
	{
		if (CU::Intersection::SphereVsSphere(sphere, myProps[i]->GetSphere()) == true && myProps[i]->GetEntity().GetComponent<HealthComponent>()->IsAlive() == true)
		{
			myProps[i]->GetEntity().GetComponent<HealthComponent>()->RemoveHealth(aDamage);
		}
	}
}

Entity* CollisionManager::GetClosestEnemyWithinSphere(const CU::Matrix44<float> &anOrientation, float aRadius)
{
	Sphere sphere;
	sphere.myCenterPosition = anOrientation.GetPos();
	sphere.myRadius = aRadius;
	sphere.myRadiusSquared = aRadius * aRadius;
	Entity* closestEnemy = nullptr;
	float bestDot = 0.f;

	for (int i = myEnemies.Size() - 1; i >= 0; --i)
	{
		if (CU::Intersection::SphereVsSphere(sphere, myEnemies[i]->GetSphere()) == true && myEnemies[i]->GetEntity().GetComponent<HealthComponent>()->IsAlive() == true)
		{
			Entity* enemy = &myEnemies[i]->GetEntity();
			
			float dotValue = CU::Dot(enemy->myOrientation.GetPos(), anOrientation.GetForward());
			if (dotValue > 0.7)
			{
				if (closestEnemy == nullptr)
				{
					closestEnemy = enemy;
					bestDot = dotValue;
					continue;
				}
				if (dotValue > bestDot)
				{
					closestEnemy = enemy;
					bestDot = dotValue;
				}
			}
		}
	}

	return closestEnemy;
}

std::string CollisionManager::GetDebugText() const
{
	std::stringstream ss;

	ss << "player " << myPlayers.Size();
	ss << std::endl << "Allies " << myAllies.Size();
	ss << std::endl << "Enemie " << myEnemies.Size();
	ss << std::endl << "Player " << myPlayerBullets.Size();
	ss << std::endl << "EnemyB " << myEnemyBullets.Size();
	ss << std::endl << "Trigge " << myTriggers.Size();
	ss << std::endl << "PowerU " << myPowerUps.Size();
	ss << std::endl << "Props; " << myProps.Size();
	ss << std::endl << "Defend " << myDefendables.Size();
	ss << std::endl << "Struct " << myStructures.Size();

	return ss.str();
}