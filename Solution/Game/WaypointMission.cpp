#include "stdafx.h"
#include "Entity.h"
#include "GUINote.h"
#include "Level.h"
#include "MissionNote.h"
#include "WaypointComponent.h"
#include "WaypointMission.h"
#include "XMLReader.h"


WaypointMission::WaypointMission(Level& aLevel, Entity& aPlayer, XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
	, myLevel(aLevel)
	, myPlayer(aPlayer)
{
	tinyxml2::XMLElement* triggerElement = aReader.ForceFindFirstChild(aElement, "trigger");
	myTrigger = myLevel.AddTrigger(aReader, triggerElement);
	myTrigger->AddComponent<WaypointComponent>();


}


WaypointMission::~WaypointMission()
{
}

bool WaypointMission::Update(float, int aMissionIndex, eMissionCategory aCategory)
{

	Prism::Engine* engine = Prism::Engine::GetInstance();
	CU::Vector2<float> screenCenter(engine->GetWindowSize().x * 0.5f, engine->GetWindowSize().y  * 0.5f);
	std::string message = "Go to waypoint";
	if (aCategory == eMissionCategory::NOT_REQUIRED)
	{
		message += " (Optional)";
	}

	PrintMissionText(message, aMissionIndex);

	return !myTrigger->GetAlive();
}

void WaypointMission::Start()
{
	myPlayer.SendNote(MissionNote(eMissionType::WAYPOINT, eMissionEvent::START));
	myPlayer.SendNote<GUINote>(GUINote(myTrigger, eGUINoteType::WAYPOINT));
}

void WaypointMission::End()
{
	myPlayer.SendNote(MissionNote(eMissionType::WAYPOINT, eMissionEvent::END));
}
