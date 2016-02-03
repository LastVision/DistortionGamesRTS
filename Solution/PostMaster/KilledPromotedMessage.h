#pragma once
#include "Message.h"
#include <GameEnum.h>

struct KilledPromotedMessage : public Message
{
	KilledPromotedMessage(eOwnerType aOwner, int aGunpowderValue);

	const eOwnerType myOwner;
	const int myGunpowderModifier;
};

inline KilledPromotedMessage::KilledPromotedMessage(eOwnerType aOwner, int aGunPowderValue)
	: Message(eMessageType::KILLED_PROMOTION)
	, myOwner(aOwner)
	, myGunpowderModifier(aGunPowderValue)
{
}