#pragma once

#include "ActorComponentData.h"
#include "AnimationComponentData.h"
#include "BuildingComponentData.h"
#include "CollisionComponentData.h"
#include "ControllerComponentData.h"
#include "EnrageComponentData.h"
#include "GraphicsComponentData.h"
#include "GrenadeComponentData.h"
#include "HealthComponentData.h"
#include "SelectionComponentData.h"
#include "TriggerComponentData.h"
#include "TotemComponentData.h"
#include "SoundComponentData.h"

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
	TotemComponentData myTotemData;
	EnrageComponentData myEnrageData;
	GrenadeComponentData myGrenadeData;
	SelectionComponentData mySelectionData;
	SoundComponentData mySoundData;

	std::string mySubType;
	eUnitType myUnitType;
};