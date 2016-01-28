#pragma once
#include <string>

struct TooltipInfo
{
	TooltipInfo(std::string aHeadline, std::string aText);

	const std::string myHeadline;
	const std::string myText;

	const int* myGunpowderCost;
	const int* myArftifactCost;
	const int* mySupplyCost;
	const float* myCooldown;

	void operator=(TooltipInfo&) = delete;
};

inline TooltipInfo::TooltipInfo(std::string aHeadline, std::string aText)
	: myHeadline(aHeadline)
	, myText(aText)
{
	myGunpowderCost = nullptr;
	myArftifactCost = nullptr;
	mySupplyCost = nullptr;
	myCooldown = nullptr;
}