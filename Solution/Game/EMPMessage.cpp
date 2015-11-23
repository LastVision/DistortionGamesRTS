#include "stdafx.h"
#include "EMPMessage.h"


EMPMessage::EMPMessage(const CU::Vector3<float>& aPosition, float aEMPTime)
	: Message(eMessageType::EMP)
	, myPosition(aPosition)
	, myEMPTime(aEMPTime)
{
}
