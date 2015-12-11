#pragma once

namespace EntityEnumConverter
{
	const eEntityType ConvertStringToEntityType(const std::string& aEntityType);
	const ePropType ConvertStringToPropType(const std::string& aPropType);
	const eUnitType ConvertStringToUnitType(const std::string& aUnitType);
}