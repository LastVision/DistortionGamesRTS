#include "stdafx.h"
#include "Engine.h"
#include "KillXEnemiesAbortMission.h"
#include "Level.h"
#include "PostMaster.h"
#include <sstream>
#include "XMLReader.h"


KillXEnemiesAbortMission::KillXEnemiesAbortMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
	, myLevel(aLevel)
	, myActive(false)
{
	tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "enemiestokillvisual");
	aReader.ForceReadAttribute(element, "value", myEnemiesToKillVisualStart);
	element = aReader.ForceFindFirstChild(aElement, "enemiestokillreal");
	aReader.ForceReadAttribute(element, "value", myEnemiesToKillRealStart);
}

KillXEnemiesAbortMission::~KillXEnemiesAbortMission()
{
	if (myActive == true)
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::ENEMY_KILLED, this);
	}
}

bool KillXEnemiesAbortMission::Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory)
{
	aDeltaTime;
	Prism::Engine* engine = Prism::Engine::GetInstance();
	CU::Vector2<float> screenCenter(engine->GetWindowSize().x * 0.5f, engine->GetWindowSize().y * 0.5f);
	std::stringstream ss;
	ss << "Kill " << myEnemiesToKillVisual << " enemies";
	if (aMissionCategory == eMissionCategory::NOT_REQUIRED)
	{
		ss << " (Optional)";
	}
	
	PrintMissionText(ss.str(), aMissionIndex);

	return myEnemiesToKillReal <= 0;
}

void KillXEnemiesAbortMission::Start()
{
	myActive = true;
	myEnemiesToKillVisual = myEnemiesToKillVisualStart;
	myEnemiesToKillReal = myEnemiesToKillRealStart;
	PostMaster::GetInstance()->Subscribe(eMessageType::ENEMY_KILLED, this);
}

void KillXEnemiesAbortMission::End()
{
	myActive = false;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ENEMY_KILLED, this);
}

void KillXEnemiesAbortMission::ReceiveMessage(const EnemyKilledMessage&)
{
	--myEnemiesToKillVisual;
	--myEnemiesToKillReal;
}
