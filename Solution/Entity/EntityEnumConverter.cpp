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
			return ePropType::PINE_TREE;
		}
		else if (aPropType == "birchtree")
		{
			return ePropType::BIRCH_TREE;
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
		else if (aPropType == "resource_pole")
		{
			return ePropType::RESOURCE_POLE;
		}
		else if (aPropType == "victory_pole")
		{
			return ePropType::VICTORY_POLE;
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