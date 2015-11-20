#include "stdafx.h"
#include "PowerUpMessage.h"


PowerUpMessage::PowerUpMessage(ePowerUpType aType, CU::Vector3<float> aPos, float aRadius, float aTime)
	: Message(eMessageType::POWER_UP)
	, myPowerupType(aType)
	, myPosition(aPos)
	, myRadius(aRadius)
	, myTime(aTime)
	, myUpgrade("")
	, myPickupMessage("")
	, myUpgradeID(-1)
	, myPickupMessageTime(0.f)
{
}

PowerUpMessage::PowerUpMessage(ePowerUpType aType, const std::string& anUpgrade, const std::string& aPickupMessage
	, int anUpgradeID, float aPickupMessageTime)
	: Message(eMessageType::POWER_UP)
	, myPowerupType(aType)
	, myPosition({0.f, 0.f, 0.f})
	, myRadius(0.f)
	, myTime(0.f)
	, myUpgrade(anUpgrade)
	, myUpgradeID(anUpgradeID)
	, myPickupMessage(aPickupMessage)
	, myPickupMessageTime(aPickupMessageTime)
{
}