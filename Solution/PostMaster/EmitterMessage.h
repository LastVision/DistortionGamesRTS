#pragma once
#include "Message.h"
#include <GameEnum.h>

struct EmitterMessage : public Message
{
	EmitterMessage(eParticleType aParticleType, int aEntityID = -1);

	const eParticleType myParticleType;
	const int myEntityID;
};

// takes in ratio, not actual position
inline EmitterMessage::EmitterMessage(eParticleType aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myParticleType(aParticleType)
	, myEntityID(aEntityID)
{
}