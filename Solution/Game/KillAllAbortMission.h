#pragma once
#include "Mission.h"

class Entity;
class Level;
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class KillAllAbortMission : public Mission
{
public:
	KillAllAbortMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory) override;

private:
	bool operator=(KillAllAbortMission&) = delete;
	Level& myLevel;
	int myAbortEnemiesLeft;
};

