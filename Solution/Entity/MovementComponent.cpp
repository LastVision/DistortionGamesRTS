#include "stdafx.h"
#include "Entity.h"
#include "MovementComponent.h"
#include <XMLReader.h>

MovementComponent::MovementComponent(Entity& aEntity, float aMovementSpeed)
	: Component(aEntity)
	, myMovementSpeed(aMovementSpeed)
{
}

MovementComponent::~MovementComponent()
{
}

void MovementComponent::Update(float aDeltaTime)
{
}

void MovementComponent::MoveTo(CU::Vector2<float> aPointToMoveTo)
{
	myEntity.myOrientation.SetPos({ aPointToMoveTo.x, 0, aPointToMoveTo.y });
}