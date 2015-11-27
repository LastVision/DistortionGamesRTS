#include "stdafx.h"
#include "CollisionComponent.h"
#include "CollisionComponentData.h"

CollisionComponent::CollisionComponent(Entity& aEntity, CollisionComponentData& aData)
	: Component(aEntity)
	, myRadius(aData.myRadius)
{
}


CollisionComponent::~CollisionComponent()
{
}