#pragma once
#include "Message.h"
#include <GameEnum.h>

struct EmitterMessage : public Message
{
	EmitterMessage(eParticleType aParticleType);

	const eParticleType myParticleType;
};

// takes in ratio, not actual position
inline EmitterMessage::EmitterMessage(eParticleType aParticleType)
	: Message(eMessageType::PARTICLE)
	, myParticleType(aParticleType)
{
}