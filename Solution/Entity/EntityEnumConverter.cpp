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
		else if (aPropType == "house_large")
		{
			return ePropType::HOUSE_LARGE;
		}
		else if (aPropType == "house_medium")
		{
			return ePropType::HOUSE_MEDIUM;
		}
		else if (aPropType == "house_small")
		{
			return ePropType::HOUSE_SMALL;
		}
		else if (aPropType == "sm_birch_tree_bare_a")
		{
			return ePropType::BIRCH_TREE_A;
		}
		else if (aPropType == "sm_pine_tree_bare_a")
		{
			return ePropType::PINE_TREE_A;
		}
		else if (aPropType == "sm_pine_tree_bare_b")
		{
			return ePropType::PINE_TREE_B;
		}
		else if (aPropType == "sm_pine_tree_bare_c")
		{
			return ePropType::PINE_TREE_C;
		}
		else if (aPropType == "sm_pine_tree_bare_d")
		{
			return ePropType::PINE_TREE_D;
		}
		else if (aPropType == "sm_house_grp")
		{
			return ePropType::HOUSE_GROUP;
		}
		else if (aPropType == "sm_house_large")
		{
			return ePropType::HOUSE_LARGE;
		}
		else if (aPropType == "sm_house_medium")
		{
			return ePropType::HOUSE_MEDIUM;
		}
		else if (aPropType == "sm_house_small")
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
		else if (aPropType == "sm_bush_a")
		{
			return ePropType::BUSH_A;
		}
		else if (aPropType == "sm_bush_b")
		{
			return ePropType::BUSH_B;
		}
		else if (aPropType == "sm_bush_c")
		{
			return ePropType::BUSH_C;
		}
		else if (aPropType == "sm_rock_large")
		{
			return ePropType::ROCK_LARGE;
		}
		else if (aPropType == "sm_rock_medium")
		{
			return ePropType::ROCK_MEDIUM;
		}
		else if (aPropType == "sm_rock_small")
		{
			return ePropType::ROCK_SMALL;
		}
		else if (aPropType == "sm_rocks_grp_a")
		{
			return ePropType::ROCKS_GROUP_A;
		}
		else if (aPropType == "sm_rocks_grp_b")
		{
			return ePropType::ROCKS_GROUP_B;
		}
		else if (aPropType == "sm_bones_medium")
		{
			return ePropType::BONES_MEDIUM;
		}
		else if (aPropType == "sm_bones_small")
		{
			return ePropType::BONES_SMALL;
		}
		else if (aPropType == "sm_brokenship_a")
		{
			return ePropType::BROKENSHIP_A;
		}
		else if (aPropType == "sm_brokenship_b")
		{
			return ePropType::BROKENSHIP_B;
		}
		else if (aPropType == "sm_brokensubmarine_a")
		{
			return ePropType::BROKENSUBMARINE_A;
		}
		else if (aPropType == "sm_car_a")
		{
			return ePropType::CAR_A;
		}
		else if (aPropType == "sm_fence_a")
		{
			return ePropType::FENCE_A;
		}
		else if (aPropType == "sm_fence_b")
		{
			return ePropType::FENCE_B;
		}
		else if (aPropType == "sm_junkpile_a")
		{
			return ePropType::JUNKPILE_A;
		}
		else if (aPropType == "sm_junkpile_b")
		{
			return ePropType::JUNKPILE_B;
		}
		else if (aPropType == "sm_log_a")
		{
			return ePropType::LOG_A;
		}
		else if (aPropType == "sm_log_grp")
		{
			return ePropType::LOG_GROUP;
		}
		else if (aPropType == "sm_oldpier_a")
		{
			return ePropType::OLDPIER_A;
		}
		else if (aPropType == "sm_pipes_a")
		{
			return ePropType::PIPES_A;
		}
		else if (aPropType == "sm_pipes_b")
		{
			return ePropType::PIPES_B;
		}
		else if (aPropType == "sm_rubble_a")
		{
			return ePropType::RUBBLE_A;
		}
		else if (aPropType == "sm_rubble_b")
		{
			return ePropType::RUBBLE_B;
		}
		else if (aPropType == "sm_rubble_c")
		{
			return ePropType::RUBBLE_C;
		}
		else if (aPropType == "sm_rubble_d")
		{
			return ePropType::RUBBLE_D;
		}
		else if (aPropType == "sm_tank_a")
		{
			return ePropType::TANK_A;
		}
		else if (aPropType == "sm_tent_a")
		{
			return ePropType::TENT_A;
		}
		else if (aPropType == "sm_tent_b")
		{
			return ePropType::TENT_B;
		}
		else if (aPropType == "sm_wheels_a")
		{
			return ePropType::WHEELS_A;
		}
		else if (aPropType == "sm_wheels_b")
		{
			return ePropType::WHEELS_B;
		}
		else if (aPropType == "sm_electric_post_a")
		{
			return ePropType::ELECTRIC_POST_A;
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
		else if (aUnitType == "non_attack_tutorial")
		{
			return eUnitType::NON_ATTACK_TUTORIAL;
		}
		DL_ASSERT("The " + aUnitType + " unit type is not supported, please tell Daniel about it.");
		return eUnitType::NOT_A_UNIT;
	}
}