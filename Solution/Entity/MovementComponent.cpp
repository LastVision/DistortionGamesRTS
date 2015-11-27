#include "stdafx.h"
#include "Entity.h"
#include "MovementComponent.h"
#include "MovementComponentData.h"
#include <XMLReader.h>

MovementComponent::MovementComponent(Entity& aEntity, MovementComponentData& aComponentData)
	: Component(aEntity)
	, myMovementSpeed(aComponentData.myMovementSpeed)
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