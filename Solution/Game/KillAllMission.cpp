#include "stdafx.h"
#include "Engine.h"
#include "KillAllMission.h"
#include "Level.h"


KillAllMission::KillAllMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
	, myLevel(aLevel)
{
}

bool KillAllMission::Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCatgory)
{
	aDeltaTime;
	Prism::Engine* engine = Prism::Engine::GetInstance();
	CU::Vector2<float> screenCenter(engine->GetWindowSize().x * 0.5f, engine->GetWindowSize().y * 0.5f);
	std::string message = "Kill all enemies";
	if (aMissionCatgory == eMissionCategory::NOT_REQUIRED)
	{
		message += " (Optional)";
	}

	PrintMissionText(message, aMissionIndex);

	return myLevel.GetEnemiesAlive() <= 0;
}
