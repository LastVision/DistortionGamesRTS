#pragma once

#include <GrowingArray.h>
#include <Subscriber.h>
#include <GameStructs.h>

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
	void RenderHealthBars(const Prism::Camera& aCamera);

	void CleanUp();

	bool SpawnUnit(eUnitType aUnitType);
	bool UpgradeUnit(eUnitType aUnitType);

	const int& GetVictoryPoints() const;
	const int& GetUnitCap() const;
	const int& GetUnitCount() const;
	
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const ResourceMessage& aMessage) override;
	void ReceiveMessage(const VictoryMessage& aMessage) override;
	void ReceiveMessage(const UpgradeUnitMessage& aMessage) override;

protected:
	bool IsAlreadyActive(Entity* aUnit);

	CU::GrowingArray<Entity*> myUnits;
	CU::GrowingArray<Entity*> myActiveUnits;
	CU::GrowingArray<Entity*> myDeadUnits;
	Entity* myBuilding;
	const eOwnerType myOwner;
	const Prism::Terrain& myTerrain;

	float myTimeMultiplier;
	int myTestGold;
	int myVictoryPoints;
	int myUnitCap;
	int myUnitCount;

private:
	void operator=(Director&) = delete;
};

inline const int& Director::GetVictoryPoints() const
{
	return myVictoryPoints;
}

inline const int& Director::GetUnitCap() const
{
	return myUnitCap;
}

inline const int& Director::GetUnitCount() const
{
	return myUnitCount;
}