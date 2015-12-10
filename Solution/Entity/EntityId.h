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

private:
	EntityId();
	~EntityId();

	CU::GrowingArray<Entity*> myEntities;

	static EntityId* myInstance;
};

