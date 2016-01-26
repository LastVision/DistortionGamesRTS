#pragma once
#include "Message.h"

namespace Prism
{
	class Scene;
}

struct KillUnitMessage : public Message
{
	KillUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aTargetPosition);

	int myUnitType;
	int myOwnerType;
	const CU::Vector2<float> myTargetPosition;
};

inline KillUnitMessage::KillUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aTargetPosition)
	: Message(eMessageType::KILL_UNIT)
	, myUnitType(aUnitType)
	, myOwnerType(aOwner)
	, myTargetPosition(aTargetPosition)
{
}