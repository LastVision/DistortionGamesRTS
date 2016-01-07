#include "stdafx.h"
#include "Tutorial.h"


Tutorial::Tutorial(const std::string& aXMLPath, const Director* aPlayer, const CU::Matrix44<float>& aCameraOrientation)
	: myPlayer(aPlayer)
	, myCameraOrientation(aCameraOrientation)
	, myMaxTime(0.5f) // read from XML later
	, myMissions(64)
	, myCurrentMission(0)
	, myMissionComplete(false)
{
	myPreviousCameraPosition = myCameraOrientation.GetPos();

	myMissions.Add(Mission("Clickable text", Action::CLICK));
	myMissions.Add(Mission("Selectable text", Action::SELECT));
	myMissions.Add(Mission("Attackable text", Action::ATTACK));
	myMissions.Add(Mission("Move camerable text", Action::MOVE_CAMERA));
	myMissions.Add(Mission("Clickable text2", Action::CLICK));
	myMissions.Add(Mission("Clickable text3", Action::CLICK));


	myCurrentTime = myMaxTime;
}


Tutorial::~Tutorial()
{
}

void Tutorial::Update(float aDeltaTime)
{
	if (myActive == false)
	{
		return;
	}

	if (myMissionComplete == false)
	{
		switch (myMissions[myCurrentMission].myAction)
		{
		case Action::ATTACK:
			break;
		case Action::CLICK:
			break;
		case Action::MOVE_CAMERA:
			break;
		case Action::SELECT:
			break;
		}


	}

	myCurrentTime -= aDeltaTime;
	if (myCurrentTime < 0)
	{
		if (myMissionComplete == true)
		{
			myCurrentTime = myMaxTime;
			++myCurrentMission;
			myMissionComplete = false;
			if (myCurrentMission == myMissions.Size())
			{
				myActive = false;
			}
			else
			{
				//set new text
			}
		}
	}
}