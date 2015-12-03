#pragma once

#include <GrowingArray.h>
#include <Subscriber.h>

class Entity;

enum class eDirectorType
{
	PLAYER,
	AI,
	COUNT
};

namespace Prism
{
	class Terrain;
}

class Director : public Subscriber
{
public:
	Director(eDirectorType aDirectorType, const Prism::Terrain& aTerrain);
	virtual ~Director();

	virtual void Update(float aDeltaTime);
	
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
protected:
	CU::GrowingArray<Entity*> myUnits;
	Entity* myBuilding;
	const eDirectorType myDirectorType;
	const Prism::Terrain& myTerrain;

private:
	void operator=(Director&) = delete;
};

