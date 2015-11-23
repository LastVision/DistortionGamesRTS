#include "stdafx.h"

#include "DefendMission.h"
#include "Enums.h"
#include "KillAllAbortMission.h"
#include "KillAllMission.h"
#include "KillStructureMission.h"
#include "KillXEnemiesAbortMission.h"
#include "KillXEnemiesMission.h"
#include <sstream>
#include "LevelScoreMessage.h"
#include "MissionContainer.h"
#include "PostMaster.h"
#include "SurvivalMission.h"
#include "SurvivalAbortMission.h"
#include "WaypointMission.h"
#include "XMLReader.h"


MissionContainer::MissionContainer(Level& aLevel, Entity& aPlayer, XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
	, myRequiredMissions(8)
	, myOptionalMissions(8)
	, myRequiredActiveMissions(8)
	, myOptionalActiveMissions(8)
	, myEndingMissions(8)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::EVENT_QUEUE_EMPTY, this);

	CU::GrowingArray<Mission*> unorderedRequiredMissions(8);
	CU::GrowingArray<Mission*> unorderedOptionalMissions(8);

	for (tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "mission"); element != nullptr;
		element = aReader.FindNextElement(element, "mission"))
	{
		std::string type;
		aReader.ForceReadAttribute(element, "type", type);
		type = CU::ToLower(type);
		int missionIndex = -1;
		aReader.ForceReadAttribute(element, "index", missionIndex);
		bool required;
		aReader.ForceReadAttribute(element, "required", required);

		Mission* mission = nullptr;
		if (type == "waypoint")
		{
			mission = new WaypointMission(aLevel, aPlayer, aReader, element);
		}
		else if (type == "killx")
		{
			mission = new KillXEnemiesMission(aLevel, aReader, element);
		}
		else if (type == "killxabort")
		{
			mission = new KillXEnemiesAbortMission(aLevel, aReader, element);
		}
		else if (type == "killall")
		{
			mission = new KillAllMission(aLevel, aReader, element);
		}
		else if (type == "killallabort")
		{
			mission = new KillAllAbortMission(aLevel, aReader, element);
		}
		else if (type == "survival")
		{
			mission = new SurvivalMission(aReader, element);
		}
		else if (type == "survivalabort")
		{
			mission = new SurvivalAbortMission(aReader, element);
		}
		else if (type == "defend")
		{
			mission = new DefendMission(aReader, element, false);
		}
		else if (type == "defendabort")
		{
			mission = new DefendMission(aReader, element, true);
		}
		else if (type == "killstructures")
		{
			mission = new KillStructureMission(aReader, element);
		}

		DL_ASSERT_EXP(mission != nullptr, "Missiontype not recognized: " + type);

		mission->SetIndex(missionIndex);

		if (required == true)
		{
			unorderedRequiredMissions.Add(mission);
		}
		else
		{
			unorderedOptionalMissions.Add(mission);
			PostMaster::GetInstance()->SendMessage<LevelScoreMessage>(LevelScoreMessage(eLevelScoreMessageType::OPTIONAL_MISSION_ADDED));
		}
	}

	DL_ASSERT_EXP(unorderedRequiredMissions.Size() > 0, "Need a required mission in missioncontainer");

	SortCopy(myRequiredMissions, unorderedRequiredMissions);
	SortCopy(myOptionalMissions, unorderedOptionalMissions);
	

	for (int i = 0; i < myRequiredMissions.Size(); ++i)
	{
		myRequiredActiveMissions.Add(myRequiredMissions[i]);
	}

	for (int i = 0; i < myOptionalMissions.Size(); ++i)
	{
		myOptionalActiveMissions.Add(myOptionalMissions[i]);
	}
}

MissionContainer::~MissionContainer()
{
	myRequiredMissions.DeleteAll();
	myOptionalMissions.DeleteAll();
	PostMaster::GetInstance()->UnSubscribe(eMessageType::EVENT_QUEUE_EMPTY, this);
}

bool MissionContainer::Update(float aDeltaTime, int, eMissionCategory)
{
	Update(aDeltaTime, myRequiredActiveMissions, 0, eMissionCategory::REQUIRED);
	Update(aDeltaTime, myOptionalActiveMissions, myRequiredActiveMissions.Size(), eMissionCategory::NOT_REQUIRED);

	return myRequiredActiveMissions.Size() <= 0 && myEndingMissions.Size() <= 0;
}

void MissionContainer::Update(float aDeltaTime, CU::GrowingArray<Mission*>& someMissions, int aOffset, eMissionCategory aCategory)
{
	for (int i = someMissions.Size() - 1; i >= 0; --i)
	{
		if (someMissions[i]->Update(aDeltaTime, i + aOffset, aCategory) == true)
		{
			if (someMissions[i]->EventsEnd() == true)
			{
				myEndingMissions.Add(someMissions[i]);
			}
			else
			{
				someMissions[i]->End();
			}
			if (aCategory == eMissionCategory::NOT_REQUIRED)
			{
				PostMaster::GetInstance()->SendMessage<LevelScoreMessage>(LevelScoreMessage(eLevelScoreMessageType::OPTIONAL_MISSION_COMPLETED));
			}
			someMissions.RemoveCyclicAtIndex(i);
		}
	}
}

void MissionContainer::Start()
{
	for (int i = 0; i < myRequiredActiveMissions.Size(); ++i)
	{
		myRequiredActiveMissions[i]->EventsStart();
		myRequiredActiveMissions[i]->Start();
	}

	for (int i = 0; i < myOptionalActiveMissions.Size(); ++i)
	{
		myOptionalActiveMissions[i]->EventsStart();
		myOptionalActiveMissions[i]->Start();
	}
}

void MissionContainer::End()
{

}

void MissionContainer::ReceiveMessage(const EventQueueEmptyMessage&)
{
	for (int i = 0; i < myEndingMissions.Size(); ++i)
	{
		myEndingMissions[i]->End();
	}

	myEndingMissions.RemoveAll();
}

void MissionContainer::SortCopy(CU::GrowingArray<Mission*>& someOut, const CU::GrowingArray<Mission*>& someIn) const
{
	int currentIndex = 0;
	while (someOut.Size() != someIn.Size())
	{
		int prevIndex = currentIndex;
		for (int i = 0; i < someIn.Size(); ++i)
		{
			if (someIn[i]->GetIndex() == currentIndex)
			{
				++currentIndex;
				someOut.Add(someIn[i]);
				break;
			}
		}
		DL_ASSERT_EXP(prevIndex == currentIndex - 1, "Mission index " + std::to_string(currentIndex) + " not found.");
	}
}
