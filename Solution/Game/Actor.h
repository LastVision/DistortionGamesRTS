#pragma once

class Entity;

enum class eActorType
{
	PLAYER,
	AI,
	COUNT
};

namespace Prism
{
	class Terrain;
}

class Actor
{
public:
	Actor(eActorType aActorType, const Prism::Terrain& aTerrain);
	virtual ~Actor();

	virtual void Update(float aDeltaTime);
	
protected:
	CU::GrowingArray<Entity*> myUnits;
	const eActorType myActorType;
	const Prism::Terrain& myTerrain;

private:
	void operator=(Actor&) = delete;
};

