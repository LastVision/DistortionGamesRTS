#pragma once

#include "ActorComponentData.h"
#include "AnimationComponentData.h"
#include "BuildingComponentData.h"
#include "CollisionComponentData.h"
#include "ControllerComponentData.h"
#include "EntityEnum.h"
#include "GraphicsComponentData.h"
#include "HealthComponentData.h"

struct EntityData
{
	eEntityType myType;
	ActorComponentData myActorData;
	AnimationComponentData myAnimationData;
	BuildingCompnentData myBuildingData;
	CollisionComponentData myCollisionData;
	ControllerComponentData myControllerData;
	GraphicsComponentData myGraphicsData;
	HealthComponentData myHealthData;
};