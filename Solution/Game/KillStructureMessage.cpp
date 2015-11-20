#include "stdafx.h"
#include "KillStructureMessage.h"


KillStructureMessage::KillStructureMessage(eType aType, const std::string& aStructureName, Entity* aEntity)
	: Message(eMessageType::KILL_STRUCTURE)
	, myType(aType)
	, myStructureName(aStructureName)
	, myEntity(aEntity)
{
}