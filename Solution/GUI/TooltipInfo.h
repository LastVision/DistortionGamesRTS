#pragma once
#include <string>

struct TooltipInfo
{
	TooltipInfo(std::string aHeadline, std::string aText);

	const std::string myHeadline;
	const std::string myText;

	int myGunpowderCost;
	int myArftifactCost;
	int mySupplyCost;
	float myCooldown;

	void operator=(TooltipInfo&) = delete;
};

inline TooltipInfo::TooltipInfo(std::string aHeadline, std::string aText)
	: myHeadline(aHeadline)
	, myText(aText)
{
	myGunpowderCost = -1;
	myArftifactCost = -1;
	mySupplyCost = -1;
	myCooldown = -1.f;
}