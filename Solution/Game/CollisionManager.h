#pragma once

#include "Enums.h"
#include <GrowingArray.h>
#include "Subscriber.h"

class CollisionComponent;
class Entity;

class CollisionManager : public Subscriber
{
public:

	CollisionManager();
	~CollisionManager();

	void Add(CollisionComponent* aComponent, eEntityType aEnum);
	void Remove(CollisionComponent* aComponent, eEntityType aEnum);

	void Update();
	void CleanUp();

	void ReceiveMessage(const PowerUpMessage& aMessage) override;

	int GetEnemiesAlive() const;
	const CU::GrowingArray<CollisionComponent*>& GetEnemies() const;

	void DisableEnemiesWithinSphere(CU::Vector3<float> aCenter, float aRadius, float aTime);
	void DamageEnemiesWithinSphere(CU::Vector3<float> aCenter, float aRadius, int aDamage);
	Entity* GetClosestEnemyWithinSphere(const CU::Matrix44<float> &anOrientation, float aRadius);

	std::string GetDebugText() const;

private:
	void CheckAllCollisions(CollisionComponent* aComponent, int aFilter);
	bool CheckCollision(CollisionComponent* aComponent, CU::GrowingArray<CollisionComponent*>& someOtherComponents);

	int myPlayerFilter;
	int myAllyFilter;
	int myEnemyFilter;
	int myPlayerBulletFilter;
	int myEnemyBulletFilter;
	int myTriggerFilter;
	int myPropFilter;
	int myPowerUpFilter;
	int myDefendableFilter;
	int myStructureFilter;

	CU::GrowingArray<CollisionComponent*> myPlayers;
	CU::GrowingArray<CollisionComponent*> myAllies;
	CU::GrowingArray<CollisionComponent*> myEnemies;
	CU::GrowingArray<CollisionComponent*> myPlayerBullets;
	CU::GrowingArray<CollisionComponent*> myEnemyBullets;
	CU::GrowingArray<CollisionComponent*> myTriggers;
	CU::GrowingArray<CollisionComponent*> myPowerUps;
	CU::GrowingArray<CollisionComponent*> myProps;
	CU::GrowingArray<CollisionComponent*> myDefendables;
	CU::GrowingArray<CollisionComponent*> myStructures;
};

inline int CollisionManager::GetEnemiesAlive() const
{
	return myEnemies.Size();
}

inline const CU::GrowingArray<CollisionComponent*>& CollisionManager::GetEnemies() const
{
	return myEnemies;
}
