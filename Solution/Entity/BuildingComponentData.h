#pragma once

struct BuildingCompnentData
{
	bool myExistsInEntity = false;
	CU::StaticArray<eUnitType, 3> myBuildUnitTypes;
	CU::StaticArray<int, 3> myUnitCosts;
};
