#pragma once

#include <GrowingArray.h>

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

class Director
{
public:
	Director(eDirectorType aDirectorType, const Prism::Terrain& aTerrain);
	virtual ~Director();

	virtual void Update(float aDeltaTime);

	void CleanUp();
	
protected:
	CU::GrowingArray<Entity*> myUnits;
	const eDirectorType myDirectorType;
	const Prism::Terrain& myTerrain;

private:
	void operator=(Director&) = delete;
};

