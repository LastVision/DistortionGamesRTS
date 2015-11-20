#pragma once

#include "Mission.h"
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class SurvivalMission : public Mission
{
public:
	SurvivalMission(XMLReader& aReader, tinyxml2::XMLElement* aElement);

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory) override;
	void Start() override;
	void End() override;

private:
	float myTime;
	float mySurvivalTime;
};

