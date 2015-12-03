#include "stdafx.h"
#include "HealthComponent.h"
#include "HealthComponentData.h"


HealthComponent::HealthComponent(Entity& aEntity, HealthComponentData& aData)
	: Component(aEntity)
	, myHealth(aData.myHealth)
{
}


HealthComponent::~HealthComponent()
{
}

