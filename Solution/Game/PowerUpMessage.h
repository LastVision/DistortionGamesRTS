#pragma once
#include "Message.h"

class PowerUpMessage : public Message
{
public:
	PowerUpMessage(ePowerUpType aType, CU::Vector3<float> aPos, float aRadius, float aTime);
	PowerUpMessage(ePowerUpType aType, const std::string& anUpgrade, const std::string& aPickupMessage
		, int anUpgradeID, float aPickupMessageTime);

	ePowerUpType GetPowerupType() const;
	const std::string& GetUprgade() const;
	const std::string& GetPickupMessage() const;
	CU::Vector3<float> GetPosition() const;
	float GetRadius() const;
	float GetTime() const;
	float GetPickupMessageTime() const;
	int GetUpgradeID() const;

private:
	ePowerUpType myPowerupType;
	std::string myUpgrade;
	std::string myPickupMessage;
	CU::Vector3<float> myPosition;
	float myRadius;

	float myDamage;
	float myTime;

	float myPickupMessageTime;

	int myUpgradeID;
};

inline ePowerUpType PowerUpMessage::GetPowerupType() const
{
	return myPowerupType;
}

inline const std::string& PowerUpMessage::GetUprgade() const
{
	return myUpgrade;
}

inline const std::string& PowerUpMessage::GetPickupMessage() const
{
	return myPickupMessage;
}

inline CU::Vector3<float> PowerUpMessage::GetPosition() const
{
	return myPosition;
}

inline float PowerUpMessage::GetRadius() const
{
	return myRadius;
}

inline float PowerUpMessage::GetTime() const
{
	return myTime;
}

inline float PowerUpMessage::GetPickupMessageTime() const
{
	return myPickupMessageTime;
}

inline int PowerUpMessage::GetUpgradeID() const
{
	return myUpgradeID;
}