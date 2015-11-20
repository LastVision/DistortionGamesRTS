#include "stdafx.h"
#include "SpawnPowerUpMessage.h"


SpawnPowerUpMessage::SpawnPowerUpMessage(const std::string& aPowerUpName, const CU::Matrix44<float>& aOrientation)
	: myPowerUpName(aPowerUpName)
	, myOrientation(aOrientation)
	, Message(eMessageType::SPAWN_POWERUP)
{
}