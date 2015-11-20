#pragma once

#include "Message.h"

class SpawnEnemyMessage : public Message
{
public:
	SpawnEnemyMessage(const std::string& aType, const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aRotation
		, const std::string& aPowerUpName);

	const std::string myType;
	const CU::Vector3<float> myPosition;
	const CU::Vector3<float> myRotation;
	const std::string myPowerUpName;

};

