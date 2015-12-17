#include "stdafx.h"

#include "EntityEnumConverter.h"

namespace EntityEnumConverter
{
	const eEntityType ConvertStringToEntityType(const std::string& aEntityType)
	{
		if (aEntityType == "basebuilding")
		{
			return eEntityType::BASE_BUILING;
		}
		else if (aEntityType == "unit")
		{
			return eEntityType::UNIT;
		}
		else if (aEntityType == "prop")
		{
			return eEntityType::PROP;
		}
		else if (aEntityType == "victorypoint")
		{
			return eEntityType::VICTORY_POINT;
		}
		else if (aEntityType == "resourcepoint")
		{
			return eEntityType::RESOURCE_POINT;
		}
		DL_ASSERT("The " + aEntityType + " entity type is not supported, please tell Daniel about it.");
		return eEntityType::_COUNT;
	}
	const ePropType ConvertStringToPropType(const std::string& aPropType)
	{
		if (aPropType == "pinetree")
		{
			return ePropType::PINE_TREE_A;
		}
		else if (aPropType == "birchtree")
		{
			return ePropType::BIRCH_TREE_A;
		}
		else if (aPropType == "SM_birch_tree_bare_a")
		{
			return ePropType::BIRCH_TREE_A;
		}
		else if (aPropType == "SM_pine_tree_bare_a")
		{
			return ePropType::PINE_TREE_A;
		}
		else if (aPropType == "SM_pine_tree_bare_b")
		{
			return ePropType::PINE_TREE_B;
		}
		else if (aPropType == "SM_pine_tree_bare_c")
		{
			return ePropType::PINE_TREE_C;
		}
		else if (aPropType == "SM_pine_tree_bare_d")
		{
			return ePropType::PINE_TREE_D;
		}
		else if (aPropType == "SM_house_grp")
		{
			return ePropType::HOUSE_GROUP;
		}
		else if (aPropType == "SM_house_large")
		{
			return ePropType::HOUSE_LARGE;
		}
		else if (aPropType == "SM_house_medium")
		{
			return ePropType::HOUSE_MEDIUM;
		}
		else if (aPropType == "SM_house_small")
		{
			return ePropType::HOUSE_SMALL;
		}
		else if (aPropType == "resource_pole")
		{
			return ePropType::RESOURCE_POLE;
		}
		else if (aPropType == "victory_pole")
		{
			return ePropType::VICTORY_POLE;
		}
		else if (aPropType == "SM_bush_a")
		{
			return ePropType::BUSH_A;
		}
		else if (aPropType == "SM_bush_b")
		{
			return ePropType::BUSH_B;
		}
		else if (aPropType == "SM_bush_c")
		{
			return ePropType::BUSH_C;
		}
		else if (aPropType == "SM_rock_large")
		{
			return ePropType::ROCK_LARGE;
		}
		else if (aPropType == "SM_rock_medium")
		{
			return ePropType::ROCK_MEDIUM;
		}
		else if (aPropType == "SM_rock_small")
		{
			return ePropType::ROCK_SMALL;
		}
		else if (aPropType == "SM_rocks_grp_a")
		{
			return ePropType::ROCKS_GROUP_A;
		}
		else if (aPropType == "SM_rocks_grp_b")
		{
			return ePropType::ROCKS_GROUP_B;
		}
		else if (aPropType == "SM_bones_medium")
		{
			return ePropType::BONES_MEDIUM;
		}
		else if (aPropType == "SM_bones_small")
		{
			return ePropType::BONES_SMALL;
		}
		else if (aPropType == "SM_brokenship_a")
		{
			return ePropType::BROKENSHIP_A;
		}
		else if (aPropType == "SM_brokenship_b")
		{
			return ePropType::BROKENSHIP_B;
		}
		else if (aPropType == "SM_brokensubmarine_a")
		{
			return ePropType::BROKENSUBMARINE_A;
		}
		else if (aPropType == "SM_car_a")
		{
			return ePropType::CAR_A;
		}
		else if (aPropType == "SM_fence_a")
		{
			return ePropType::FENCE_A;
		}
		else if (aPropType == "SM_fence_b")
		{
			return ePropType::FENCE_B;
		}
		else if (aPropType == "SM_junkpile_a")
		{
			return ePropType::JUNKPILE_A;
		}
		else if (aPropType == "SM_junkpile_b")
		{
			return ePropType::JUNKPILE_B;
		}
		else if (aPropType == "SM_log_a")
		{
			return ePropType::LOG_A;
		}
		else if (aPropType == "SM_log_grp")
		{
			return ePropType::LOG_GROUP;
		}
		else if (aPropType == "SM_oldpier_a")
		{
			return ePropType::OLDPIER_A;
		}
		else if (aPropType == "SM_pipes_a")
		{
			return ePropType::PIPES_A;
		}
		else if (aPropType == "SM_pipes_b")
		{
			return ePropType::PIPES_B;
		}
		else if (aPropType == "SM_rumble_a")
		{
			return ePropType::RUMBLE_A;
		}
		else if (aPropType == "SM_rumble_b")
		{
			return ePropType::RUMBLE_B;
		}
		else if (aPropType == "SM_rumble_c")
		{
			return ePropType::RUMBLE_C;
		}
		else if (aPropType == "SM_rumble_d")
		{
			return ePropType::RUMBLE_D;
		}
		else if (aPropType == "SM_tank_a")
		{
			return ePropType::TANK_A;
		}
		else if (aPropType == "SM_tent_a")
		{
			return ePropType::TENT_A;
		}
		else if (aPropType == "SM_tent_b")
		{
			return ePropType::TENT_B;
		}
		else if (aPropType == "SM_wheels_a")
		{
			return ePropType::WHEELS_A;
		}
		else if (aPropType == "SM_wheels_b")
		{
			return ePropType::WHEELS_B;
		}
		DL_ASSERT("The " + aPropType + " prop type is not supported, please tell Daniel about it.");
		return ePropType::NOT_A_PROP;
	}
	const eUnitType ConvertStringToUnitType(const std::string& aUnitType)
	{
		if (aUnitType == "grunt")
		{
			return eUnitType::GRUNT;
		}
		else if (aUnitType == "ranger")
		{
			return eUnitType::RANGER;
		}
		else if (aUnitType == "tank")
		{
			return eUnitType::TANK;
		}
		DL_ASSERT("The " + aUnitType + " unit type is not supported, please tell Daniel about it.");
		return eUnitType::NOT_A_UNIT;
	}
}