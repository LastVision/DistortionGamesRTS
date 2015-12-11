#include "stdafx.h"

#include "Entity.h"
#include "EntityId.h"
#include <MemoryTracker.h>
#include "PollingStation.h"

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
	SAFE_DELETE(myInstance);
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

		if (anEntity->GetType() == eEntityType::RESOURCE_POINT)
		{
			myTriggers.Add(anEntity);
			PollingStation::GetInstance()->RegisterEntity(anEntity);
		}
	}
	return index;
}

Entity* EntityId::GetEntity(int aId) const
{
	return myEntities[aId];
}

Entity* EntityId::GetTrigger(int aId) const
{
	return myTriggers[aId];
}

EntityId::EntityId()
	: myEntities(4096)
	, myTriggers(GC::triggerCount)
{
}


EntityId::~EntityId()
{
}
