#pragma once
#include "EntityEnum.h"
#include <StaticArray.h>

struct BuildingCompnentData
{
	bool myExistsInEntity = false;
	CU::StaticArray<eEntityType, 3> myBuildUnitTypes;
};
