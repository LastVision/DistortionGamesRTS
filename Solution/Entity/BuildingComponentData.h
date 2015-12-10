#pragma once
#include "EntityEnum.h"
#include <StaticArray.h>

struct BuildingCompnentData
{
	bool myExistsInEntity = false;
	CU::StaticArray<eUnitType, 3> myBuildUnitTypes;
};
