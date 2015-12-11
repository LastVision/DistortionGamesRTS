#pragma once
#include "Message.h"

namespace Prism
{
	class Scene;
}

struct SpawnUnitMessage : public Message
{
	SpawnUnitMessage(int aUnitType, int aOwner/*, Prism::Scene& aScene*/);

	int myUnitType;
	int myOwnerType;

	//Prism::Scene& myScene;
};

// takes in ratio, not actual position
inline SpawnUnitMessage::SpawnUnitMessage(int aUnitType, int aOwner/*, Prism::Scene& aScene*/)
	: Message(eMessageType::SPAWN_UNIT)
	, myUnitType(aUnitType)
	, myOwnerType(aOwner)
	//, myScene(aScene)
{
}