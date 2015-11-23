#include "stdafx.h"
#include <sstream>
#include "SurvivalMission.h"
#include "XMLReader.h"


SurvivalMission::SurvivalMission(XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
{
	tinyxml2::XMLElement* element= aReader.ForceFindFirstChild(aElement, "seconds");
	aReader.ForceReadAttribute(element, "value", mySurvivalTime);
}

bool SurvivalMission::Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory)
{
	std::stringstream ss;
	ss.precision(2);
	ss << "Survive for: " << myTime << " seconds";

	if (aMissionCategory == eMissionCategory::NOT_REQUIRED)
	{
		ss << " (Optional)";
	}

	PrintMissionText(ss.str(), aMissionIndex);

	myTime -= aDeltaTime;
	return myTime <= 0.f;
}

void SurvivalMission::Start()
{
	myTime = mySurvivalTime;
}

void SurvivalMission::End()
{
	myTime = 0.f;
}
