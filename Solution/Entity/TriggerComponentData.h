#pragma once
#include "EntityEnum.h"

struct TriggerComponentData
{
	float myRadius;
	eTriggerType myType;
	bool myExistsInEntity = false;
};