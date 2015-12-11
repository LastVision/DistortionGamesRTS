#pragma once
#include <GrowingArray.h>
#include <Vector.h>
#include "EntityEnum.h"

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Destroy();

	void RegisterEntity(Entity* aEntity);

	Entity* FindClosestEntity(const CU::Vector3<float>& aPosition, eOwnerType aEntityOwner, float aMaxDistance = FLT_MAX);
	Entity* FindEntityAtPosition(const CU::Vector3<float>& aPosition, eOwnerType aEntityOwner);
	
	const CU::GrowingArray<Entity*>& GetUnits(eOwnerType anOwner) const;

	void CleanUp();



private:
	PollingStation();
	~PollingStation();


	CU::GrowingArray<Entity*> myPlayerUnits;
	CU::GrowingArray<Entity*> myAIUnits;

	static PollingStation* myInstance;
};

