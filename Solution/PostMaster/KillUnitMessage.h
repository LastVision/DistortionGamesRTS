#pragma once
#include "Message.h"

namespace Prism
{
	class Scene;
}

struct KillUnitMessage : public Message
{
	KillUnitMessage(int aUnitType, int aOwner);

	int myUnitType;
	int myOwnerType;
};

inline KillUnitMessage::KillUnitMessage(int aUnitType, int aOwner)
	: Message(eMessageType::KILL_UNIT)
	, myUnitType(aUnitType)
	, myOwnerType(aOwner)
{
}