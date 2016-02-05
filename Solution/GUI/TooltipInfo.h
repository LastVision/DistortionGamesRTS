#pragma once
#include <string>

struct TooltipInfo
{
	TooltipInfo(std::string aHeadline, std::string aText, bool anIsLargeTooltip);

	const std::string myHeadline;
	const std::string myText;
	const bool myIsLargeTooltip;

	const int* myGunpowderCost;
	const int* myArftifactCost;
	const int* mySupplyCost;
	const float* myCooldown;

	void operator=(TooltipInfo&) = delete;
};

inline TooltipInfo::TooltipInfo(std::string aHeadline, std::string aText, bool anIsLargeTooltip)
	: myHeadline(aHeadline)
	, myText(aText)
	, myIsLargeTooltip(anIsLargeTooltip)
{
	myGunpowderCost = nullptr;
	myArftifactCost = nullptr;
	mySupplyCost = nullptr;
	myCooldown = nullptr;
}