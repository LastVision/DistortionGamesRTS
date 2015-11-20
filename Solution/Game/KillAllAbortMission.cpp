#include "stdafx.h"
#include "Engine.h"
#include "KillAllAbortMission.h"
#include "Level.h"
#include "XMLReader.h"


KillAllAbortMission::KillAllAbortMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
	, myLevel(aLevel)
{
	tinyxml2::XMLElement* enemiesLeftElement = aReader.ForceFindFirstChild(aElement, "enemiesleft");
	aReader.ForceReadAttribute(enemiesLeftElement, "value", myAbortEnemiesLeft);
}

bool KillAllAbortMission::Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory)
{
	aDeltaTime;
	Prism::Engine* engine = Prism::Engine::GetInstance();
	CU::Vector2<float> screenCenter(engine->GetWindowSize().x * 0.5f, engine->GetWindowSize().y * 0.5f);
	std::string message = "Kill all enemies!";
	if (aMissionCategory == eMissionCategory::NOT_REQUIRED)
	{
		message += " (Optional)";
	}
	PrintMissionText(message, aMissionIndex);

	return myLevel.GetEnemiesAlive() <= myAbortEnemiesLeft;
}
