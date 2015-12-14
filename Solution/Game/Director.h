#pragma once

#include <GrowingArray.h>
#include <Subscriber.h>

class Entity;

namespace Prism
{
	class Terrain;
}

class Director : public Subscriber
{
	friend class LevelFactory;
public:
	Director(eOwnerType aOwnerType, const Prism::Terrain& aTerrain);
	virtual ~Director();

	virtual void Update(float aDeltaTime);

	void CleanUp();

	int GetUnitCount() const;
	
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const ResourceMessage& aMessage) override;
	void ReceiveMessage(const VictoryMessage& aMessage) override;
protected:
	CU::GrowingArray<Entity*> myUnits;
	CU::GrowingArray<Entity*> myActiveUnits;
	Entity* myBuilding;
	const eOwnerType myOwner;
	const Prism::Terrain& myTerrain;

	float myTimeMultiplier;
	int myTestGold;
	int myVictoryPoints;

private:
	void operator=(Director&) = delete;
};

inline int Director::GetUnitCount() const
{
	return myActiveUnits.Size();
}
