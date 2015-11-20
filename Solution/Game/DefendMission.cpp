#include "stdafx.h"
#include "DefendMessage.h"
#include "DefendMission.h"
#include "Engine.h"
#include "Entity.h"
#include "HealthComponent.h"
#include "Level.h"
#include "PostMaster.h"
#include <sstream>
#include <XMLReader.h>


DefendMission::DefendMission(XMLReader& aReader, tinyxml2::XMLElement* aElement, bool aAbortMission)
	: Mission(aReader, aElement)
	, myRealTimeStart(0)
	, myVisualTimeStart(0)
	, myAbortMission(aAbortMission)
	, myEntityToDefend(nullptr)
{
	tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "defend");
	aReader.ForceReadAttribute(element, "defendName", myNameToDefend);
	myNameToDefend = CU::ToLower(myNameToDefend);

	if (myAbortMission == true)
	{
		element = aReader.ForceFindFirstChild(aElement, "secondsvisual");
		aReader.ForceReadAttribute(element, "value", myVisualTimeStart);
		element = aReader.ForceFindFirstChild(aElement, "secondsreal");
		aReader.ForceReadAttribute(element, "value", myRealTimeStart);
	}
	else
	{
		element = aReader.ForceFindFirstChild(aElement, "seconds");
		aReader.ForceReadAttribute(element, "value", myRealTimeStart);
		myVisualTimeStart = myRealTimeStart;
	}
}

DefendMission::~DefendMission()
{
	if (PostMaster::GetInstance()->IsSubscribed(eMessageType::DEFEND, this) == true)
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::DEFEND, this);
	}
}

void DefendMission::Start()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::DEFEND, this);
	PostMaster::GetInstance()->SendMessage<DefendMessage>(DefendMessage(DefendMessage::eType::NAME, myNameToDefend));
	myRealTime = myRealTimeStart;
	myVisualTime = myVisualTimeStart;
}

bool DefendMission::Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory)
{
	DL_ASSERT_EXP(myEntityToDefend != nullptr, "Could not find entity to defend: " + myNameToDefend);

	std::stringstream ss;
	ss.precision(2);
	int health = myEntityToDefend->GetComponent<HealthComponent>()->GetHealth();
	
	ss << "Defend for " << myVisualTime << " s, " << health << " hp";
	
	if (aMissionCategory == eMissionCategory::NOT_REQUIRED)
	{
		ss << " (Optional)";
	}

	PrintMissionText(ss.str(), aMissionIndex);

	myRealTime -= aDeltaTime;
	myVisualTime -= aDeltaTime;
	return myRealTime <= 0.f;
}

void DefendMission::End()
{
	myRealTime = 0.f;
	myVisualTime = 0.f;
	PostMaster::GetInstance()->SendMessage<DefendMessage>(DefendMessage(DefendMessage::eType::COMPLETE, myNameToDefend));
	PostMaster::GetInstance()->UnSubscribe(eMessageType::DEFEND, this);
}

void DefendMission::ReceiveMessage(const DefendMessage& aMessage)
{
	if (aMessage.myType == DefendMessage::eType::ENTITY)
	{
		myEntityToDefend = aMessage.myEntity;
	}
}