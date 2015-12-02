#include "stdafx.h"
#include "ActorComponent.h"


ActorComponent::ActorComponent(Entity& aEntity, ActorComponentData& aData)
	: Component(aEntity)
{
	DL_ASSERT("NOT IMPLEMENTED");
}


ActorComponent::~ActorComponent()
{
}

void ActorComponent::Update(float aDelta)
{

}
