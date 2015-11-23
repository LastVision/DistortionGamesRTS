#pragma once

#include "Mission.h"
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class SurvivalAbortMission : public Mission
{
public:
	SurvivalAbortMission(XMLReader& aReader, tinyxml2::XMLElement* aElement);

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory) override;
	void Start() override;
	void End() override;

private:
	float myVisualTime;
	float myVisualTimeStart;
	float myRealTime;
	float myRealTimeStart;
};

