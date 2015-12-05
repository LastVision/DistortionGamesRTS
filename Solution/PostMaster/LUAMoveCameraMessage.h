#pragma once
#include "Message.h"
#include "Vector.h"

struct LUAMoveCameraMessage : public Message
{
	LUAMoveCameraMessage(CU::Vector2<float> aMoveAmount);

	const CU::Vector2<float> myMoveAmount;

};

inline LUAMoveCameraMessage::LUAMoveCameraMessage(CU::Vector2<float> aMoveAmount)
	: Message(eMessageType::LUA_MOVE_CAMERA)
	, myMoveAmount(aMoveAmount)
{
}