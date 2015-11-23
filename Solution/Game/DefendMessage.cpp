#include "stdafx.h"
#include "DefendMessage.h"


DefendMessage::DefendMessage(eType aType, const std::string& aDefendName, Entity* aEntity)
	: Message(eMessageType::DEFEND)
	, myType(aType)
	, myDefendName(aDefendName)
	, myEntity(aEntity)
{
}