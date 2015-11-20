#include "stdafx.h"
#include "Engine.h"
#include "KillXEnemiesMission.h"
#include "Level.h"
#include "PostMaster.h"
#include <sstream>
#include "XMLReader.h"


KillXEnemiesMission::KillXEnemiesMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
	, myLevel(aLevel)
	, myActive(false)
{
	tinyxml2::XMLElement* enemiesToKillElement = aReader.ForceFindFirstChild(aElement, "enemiestokill");
	aReader.ForceReadAttribute(enemiesToKillElement, "value", myEnemiesToKillStart);
}

KillXEnemiesMission::~KillXEnemiesMission()
{
	if (myActive == true)
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::ENEMY_KILLED, this);
	}
}

bool KillXEnemiesMission::Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory)
{
	aDeltaTime;
	Prism::Engine* engine = Prism::Engine::GetInstance();
	CU::Vector2<float> screenCenter(engine->GetWindowSize().x * 0.5f, engine->GetWindowSize().y * 0.5f);
	std::stringstream ss;
	ss << "Kill " << myEnemiesToKill << " enemies.";
	if (aMissionCategory == eMissionCategory::NOT_REQUIRED)
	{
		ss << " (Optional)";
	}

	PrintMissionText(ss.str(), aMissionIndex);
	
	return myEnemiesToKill <= 0;
}

void KillXEnemiesMission::Start()
{
	myActive = true;
	myEnemiesToKill = myEnemiesToKillStart;
	PostMaster::GetInstance()->Subscribe(eMessageType::ENEMY_KILLED, this);
}

void KillXEnemiesMission::End()
{
	myActive = false;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ENEMY_KILLED, this);
}

void KillXEnemiesMission::ReceiveMessage(const EnemyKilledMessage&)
{
	--myEnemiesToKill;
}
