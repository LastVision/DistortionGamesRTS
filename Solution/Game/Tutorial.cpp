#include "stdafx.h"

#include <InputWrapper.h>
#include "NeutralDirector.h"
#include "PlayerDirector.h"
#include <PostMaster.h>
#include "Tutorial.h"
#include <Text.h>
#include <TextMessage.h>
#include <XMLReader.h>

Tutorial::Tutorial(const std::string& aXMLPath, const PlayerDirector* aPlayer, const CU::Matrix44<float>& aCameraOrientation
		, const NeutralDirector* aNeutral)
	: myPlayer(aPlayer)
	, myCameraOrientation(aCameraOrientation)
	, myNeutral(aNeutral)
	, myMaxTime(3.0f) // read from XML later
	, myMissions(64)
	, myCurrentMission(0)
	, myMissionComplete(false)
	, myActive(true)
{
	myPreviousNeutralCount = myNeutral->GetUnitCount();
	myPreviousCameraPosition = myCameraOrientation.GetPos();


	XMLReader reader;
	reader.OpenDocument(aXMLPath);

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");
	element = reader.ForceFindFirstChild(element, "tutorial");
	for (tinyxml2::XMLElement* mission = reader.FindFirstChild(element, "mission"); mission != nullptr
		; mission = reader.FindNextElement(mission, "mission"))
	{
		std::string typeString;
		reader.ForceReadAttribute(mission, "type", typeString);

		std::string textString = reader.ForceFindFirstChild(mission, "text")->GetText();

		Prism::Text* text = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
		text->SetText(textString);
		myMissions.Add(Mission(text, GetAction(typeString)));
	}
	reader.CloseDocument();

	if (myMissions.Size() > 0)
	{
		PostMaster::GetInstance()->SendMessage(TextMessage(myMissions[myCurrentMission].myText));
	}

	myCurrentTime = myMaxTime;
}

Tutorial::~Tutorial()
{
	for (int i = 0; i < myMissions.Size(); ++i)
	{
		SAFE_DELETE(myMissions[i].myText);
	}
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
			if (myNeutral->GetUnitCount() < myPreviousNeutralCount)
			{
				myMissionComplete = true;
			}
			break;
		case Action::CLICK:
			if (CU::InputWrapper::GetInstance()->MouseUp(0) == true)
			{
				myMissionComplete = true;
			}
			break;
		case Action::MOVE_CAMERA:
			if (myCameraOrientation.GetPos() != myPreviousCameraPosition)
			{
				myMissionComplete = true;
			}
			break;
		case Action::SELECT:
			if (myPlayer->GetSelectedUnits().Size() > 0)
			{
				myMissionComplete = true;
			}
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

			myPreviousNeutralCount = myNeutral->GetUnitCount();
			myPreviousCameraPosition = myCameraOrientation.GetPos();

			if (myCurrentMission == myMissions.Size())
			{
				PostMaster::GetInstance()->SendMessage(TextMessage(nullptr));
				myActive = false;
			}
			else
			{
				PostMaster::GetInstance()->SendMessage(TextMessage(myMissions[myCurrentMission].myText));
			}
		}
	}
}

Tutorial::Action Tutorial::GetAction(const std::string& anAction) const
{
	if (anAction == "click")
	{
		return Action::CLICK;
	}
	else if(anAction == "select")
	{
		return Action::SELECT;
	}
	else if (anAction == "move_camera")
	{
		return Action::MOVE_CAMERA;
	}
	else if (anAction == "attack")
	{
		return Action::ATTACK;
	}

	DL_ASSERT("Action not found.");
	return Action::CLICK;
}