#pragma once
#include "AnimationComponentData.h"
#include "EntityEnum.h"
#include "GraphicsComponentData.h"
#include "MovementComponentData.h"
#include "CollisionComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	MovementComponentData myMovementData;
	CollisionComponentData myCollisionData;
};