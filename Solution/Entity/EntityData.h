#pragma once

#include "ActorComponentData.h"
#include "AnimationComponentData.h"
#include "BuildingComponentData.h"
#include "CollisionComponentData.h"
#include "ControllerComponentData.h"
#include "GraphicsComponentData.h"
#include "HealthComponentData.h"
#include "TriggerComponentData.h"

struct EntityData
{
	eEntityType myType;
	ActorComponentData myActorData;
	AnimationComponentData myAnimationData;
	BuildingComponentData myBuildingData;
	CollisionComponentData myCollisionData;
	ControllerComponentData myControllerData;
	GraphicsComponentData myGraphicsData;
	HealthComponentData myHealthData;
	TriggerComponentData myTriggerData;

	union 
	{
		ePropType myPropType;
		eUnitType myUnitType;
	};
};