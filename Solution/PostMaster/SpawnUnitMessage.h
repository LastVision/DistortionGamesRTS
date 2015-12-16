#pragma once
#include "Message.h"

namespace Prism
{
	class Scene;
}

struct SpawnUnitMessage : public Message
{
	SpawnUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aPosition = CU::Vector2<float>(-1.f, -1.f));

	int myUnitType;
	int myOwnerType;
	CU::Vector2<float> myPosition;
};

inline SpawnUnitMessage::SpawnUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aPosition)
	: Message(eMessageType::SPAWN_UNIT)
	, myUnitType(aUnitType)
	, myOwnerType(aOwner)
	, myPosition(aPosition)
{
}