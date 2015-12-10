#pragma once
#include <GrowingArray.h>

class Entity;

class EntityId
{
public:
	static EntityId* GetInstance();
	static void Destroy();

	int GetId(Entity* anEntity);
	Entity* GetEntity(int aId) const;
	Entity* GetTrigger(int aId) const;

private:
	EntityId();
	~EntityId();

	CU::GrowingArray<Entity*> myEntities;
	CU::GrowingArray<Entity*> myTriggers;

	static EntityId* myInstance;
};

