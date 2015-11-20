#pragma once
#include "Message.h"


class EMPMessage : public Message
{
public:
	EMPMessage(const CU::Vector3<float>& aPosition, float aEMPTime);

	const CU::Vector3<float> myPosition;
	const float myEMPTime;
};

