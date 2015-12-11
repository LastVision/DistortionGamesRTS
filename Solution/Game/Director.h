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
	friend class LevelFactory;
public:
	Director(eDirectorType aDirectorType, const Prism::Terrain& aTerrain);
	virtual ~Director();

	virtual void Update(float aDeltaTime);

	void CleanUp();

	int GetUnitCount() const;
	
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
protected:
	CU::GrowingArray<Entity*> myUnits;
	CU::GrowingArray<Entity*> myActiveUnits;
	Entity* myBuilding;
	const eDirectorType myDirectorType;
	const Prism::Terrain& myTerrain;

	float myTimeMultiplier;

private:
	void operator=(Director&) = delete;
};

inline int Director::GetUnitCount() const
{
	return myActiveUnits.Size();
}
