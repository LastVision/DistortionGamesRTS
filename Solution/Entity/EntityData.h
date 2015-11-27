#pragma once
#include "AnimationComponentData.h"
#include "EntityEnum.h"
#include "GraphicsComponentData.h"
#include "MovementComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	MovementComponentData myMovementData;
};