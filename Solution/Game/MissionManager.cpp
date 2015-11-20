#include "stdafx.h"

#include <AudioInterface.h>
#include "CommonHelper.h"
#include <DL_Assert.h>
#include "Entity.h"
#include "Level.h"
#include "MissionContainer.h"
#include "MissionManager.h"
#include "PostMaster.h"
#include <sstream>
#include "XMLReader.h"


MissionManager::MissionManager(Level& aLevel, Entity& aPlayer, const std::string& aFileToReadFrom)
	: myLevel(aLevel)
	, myPlayer(aPlayer)
	, myMissions(16)
	, myCurrentMission(0) 
	, myMissionsNotOrder(16)
	, myAllowedToStartNextMission(true)
	, myEndEventsActive(false)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::EVENT_QUEUE_EMPTY, this);
	XMLReader reader;
	reader.OpenDocument(aFileToReadFrom);

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	for (element = reader.ForceFindFirstChild(element, "missionContainer"); element != nullptr;
		element = reader.FindNextElement(element, "missionContainer"))
	{
		int missionIndex = -1;
		reader.ForceReadAttribute(element, "index", missionIndex);

		MissionContainer* mission = new MissionContainer(myLevel, myPlayer, reader, element);
		mission->SetIndex(missionIndex);
		myMissionsNotOrder.Add(mission);
	}
	reader.CloseDocument();

	int currentIndex = 0;
	while (myMissions.Size() != myMissionsNotOrder.Size())
	{
		int prevIndex = currentIndex;
		for (int i = 0; i < myMissionsNotOrder.Size(); ++i)
		{
			if (myMissionsNotOrder[i]->GetIndex() == currentIndex)
			{
				++currentIndex;
				myMissions.Add(myMissionsNotOrder[i]);
				break;
			}
		}
		DL_ASSERT_EXP(prevIndex == currentIndex - 1, "Mission index " + std::to_string(currentIndex) + " not found.");
	}
}

MissionManager::~MissionManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::EVENT_QUEUE_EMPTY, this);
	myMissions.DeleteAll();
}

void MissionManager::Init()
{
	myMissions[myCurrentMission]->EventsStart();
	myMissions[myCurrentMission]->Start();
	myAllowedToStartNextMission = false;
	myEndEventsActive = false;
}

void MissionManager::Update(float aDeltaTime, bool aForceNextMission)
{
	DL_ASSERT_EXP(myCurrentMission < myMissions.Size(), "CurrentMission out of bounds!");
	std::stringstream ss;
	ss << myCurrentMission;
	Prism::Engine::GetInstance()->PrintText(ss.str(), { 400, -370 }, Prism::eTextType::DEBUG_TEXT);
	if (aForceNextMission == true || myEndEventsActive == false && myMissions[myCurrentMission]->Update(aDeltaTime, -1, eMissionCategory::DUMMY) == true)
	{
		myAllowedToStartNextMission = !myMissions[myCurrentMission]->EventsEnd();
		myEndEventsActive = true;
	}

	if (myEndEventsActive == true && myAllowedToStartNextMission == true)
	{
		myMissions[myCurrentMission]->End();
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_MissionCompleted", 0);
		++myCurrentMission;
		if (myCurrentMission == myMissions.Size())
		{
			myLevel.CompleteLevel();
		}
		else
		{
			myMissions[myCurrentMission]->EventsStart();
			myMissions[myCurrentMission]->Start();
			myAllowedToStartNextMission = false;
			myEndEventsActive = false;
		}
	}
}

void MissionManager::ReceiveMessage(const EventQueueEmptyMessage&)
{
	myAllowedToStartNextMission = true;
}