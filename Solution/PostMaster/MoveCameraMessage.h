#pragma once
#include "Message.h"
#include "Vector.h"

struct MoveCameraMessage : public Message
{
	MoveCameraMessage(CU::Vector2<float> aPosition);

	CU::Vector2<float> myPosition;
	
};

// takes in ratio, not actual position
inline MoveCameraMessage::MoveCameraMessage(CU::Vector2<float> aPosition)
	: Message(eMessageType::MOVE_CAMERA)
	, myPosition(aPosition)
{
}