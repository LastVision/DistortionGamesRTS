#pragma once
#include "Director.h"

namespace Prism
{
	class Camera;
	class Terrain;
	class Scene;
}

class AIDirector : public Director
{
public:
	AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene);
	~AIDirector();

	void Update(float aDeltaTime) override;

	void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const AITimeMultiplierMessage& aMessage) override;

private:
	enum eUnitActionType
	{
		GATHERER,
		ATTACKER
	};

	void CleanUpGatherers();
	void CleanUpAttackers();

	void ActivateGatherer(Entity* aEntity);
	void ActivateAttacker(Entity* aEntity);

	float mySpawnTimer;
	int myOptimalGathererCount;
	int myOptimalAttackerCount;
	int myCurrentAttackerCount;
	int myCurrentGathererCount;

	CU::GrowingArray<Entity*> myPointOneGatherers;
	CU::GrowingArray<Entity*> myPointTwoGatherers;
	CU::GrowingArray<Entity*> myAttackers;

	CU::GrowingArray<eUnitActionType> myUnitQueue;
};

