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
		DL_ASSERT("This entity type is not supported, please tell Daniel about it.");
		return eEntityType::_COUNT;
	}
	const ePropType ConvertStringToPropType(const std::string& aPropType)
	{
		if (aPropType == "pinetree")
		{
			return ePropType::PINE_TREE;
		}
		DL_ASSERT("This prop type is not supported, please tell Daniel about it.");
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
		else if (aUnitType == "scout")
		{
			return eUnitType::SCOUT;
		}
		DL_ASSERT("This unit type is not supported, please tell Daniel about it.");
		return eUnitType::NOT_A_UNIT;
	}
}