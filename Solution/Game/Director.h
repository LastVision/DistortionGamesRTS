#pragma once

#include <GrowingArray.h>
#include <Subscriber.h>
#include <GameStructs.h>

class Entity;
class Formation;

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

	int GetUnitCount() const;
	bool SpawnUnit(eUnitType aUnitType);
	bool UpgradeUnit(eUnitType aUnitType);

	const int& GetVictoryPoints() const;
	
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const ResourceMessage& aMessage) override;
	void ReceiveMessage(const VictoryMessage& aMessage) override;
	void ReceiveMessage(const UpgradeUnitMessage& aMessage) override;

protected:
	bool IsAlreadyActive(Entity* aUnit);

	CU::GrowingArray<Entity*> myUnits;
	CU::GrowingArray<Entity*> myActiveUnits;
	CU::GrowingArray<Entity*> myDeadUnits;
	CU::GrowingArray<Formation*> myFormations;
	CU::GrowingArray<Formation*> myActiveFormations;
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

inline const int& Director::GetVictoryPoints() const
{
	return myVictoryPoints;
}