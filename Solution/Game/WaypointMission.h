#pragma once

#include "Mission.h"
class Entity;
class Level;
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class WaypointMission :	public Mission
{
public:
	WaypointMission(Level& aLevel, Entity& aPlayer, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	~WaypointMission();

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aCategory) override;
	void Start() override;
	void End() override;

private:
	bool operator=(WaypointMission&) = delete;


	Level& myLevel;
	Entity* myTrigger;
	Entity& myPlayer;
};

