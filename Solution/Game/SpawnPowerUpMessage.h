#pragma once
#include "Message.h"

class SpawnPowerUpMessage : public Message
{
public:
	SpawnPowerUpMessage(const std::string& aPowerUpName, const CU::Matrix44<float>& aOrientation);

	const std::string myPowerUpName;
	const CU::Matrix44<float> myOrientation;
};

