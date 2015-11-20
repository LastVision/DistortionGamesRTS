#include "stdafx.h"
#include "WaypointComponent.h"
#include "CollisionNote.h"
#include "Entity.h"

WaypointComponent::WaypointComponent(Entity& aEntity)
	: Component(aEntity)
{
}

void WaypointComponent::ReceiveNote(const CollisionNote& aNote)
{
	if (aNote.myEntity.GetAlive() == true)
	{
		myEntity.Kill();
	}
}