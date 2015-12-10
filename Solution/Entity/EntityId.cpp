#include "stdafx.h"

#include "EntityId.h"
#include <MemoryTracker.h>

EntityId* EntityId::myInstance = nullptr;

EntityId* EntityId::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new EntityId();
	}

	return myInstance;
}

void EntityId::Destroy()
{
	delete myInstance;
}

int EntityId::GetId(Entity* anEntity)
{
	DL_ASSERT_EXP(Prism::MemoryTracker::GetInstance()->GetRunTime() == false
		, "Don't call EntityId in runtime, too expensive.");
	int index = myEntities.Find(anEntity);
	if (index < 0)
	{
		myEntities.Add(anEntity);
		index = myEntities.Size() - 1;
	}
	return index;
}

Entity* EntityId::GetEntity(int aId) const
{
	return myEntities[aId];
}

EntityId::EntityId()
	: myEntities(4096)
{
}


EntityId::~EntityId()
{
}
