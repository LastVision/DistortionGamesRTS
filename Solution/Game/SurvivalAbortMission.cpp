#include "stdafx.h"
#include <sstream>
#include "SurvivalAbortMission.h"
#include "XMLReader.h"


SurvivalAbortMission::SurvivalAbortMission(XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
{
	tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "secondsvisual");
	aReader.ForceReadAttribute(element, "value", myVisualTimeStart);
	element = aReader.ForceFindFirstChild(aElement, "secondsreal");
	aReader.ForceReadAttribute(element, "value", myRealTimeStart);
}

bool SurvivalAbortMission::Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory)
{
	std::stringstream ss;
	ss.precision(2);
	ss << "Survive for: " << myVisualTime << " seconds";

	if (aMissionCategory == eMissionCategory::NOT_REQUIRED)
	{
		ss << " (Optional)";
	}

	PrintMissionText(ss.str(), aMissionIndex);

	myVisualTime -= aDeltaTime;
	myRealTime -= aDeltaTime;
	return myRealTime <= 0;
}

void SurvivalAbortMission::Start()
{
	myVisualTime = myVisualTimeStart;
	myRealTime = myRealTimeStart;
}

void SurvivalAbortMission::End()
{
	myVisualTime = 0.f;
	myRealTime = 0.f;
}
