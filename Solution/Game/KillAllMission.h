#pragma once
#include "Mission.h"

class Entity;
class Level;
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class KillAllMission : public Mission
{
public:
	KillAllMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCatgory) override;

private:
	bool operator=(KillAllMission&) = delete;
	Level& myLevel;
};

