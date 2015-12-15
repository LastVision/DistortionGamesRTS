#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <Vector.h>

struct EmitterMessage : public Message
{
	EmitterMessage(eParticleType aParticleType, int aEntityID);
	EmitterMessage(eParticleType aParticleType, const CU::Vector3f& aPosition);

	const eParticleType myParticleType;
	const int myEntityID;
	const CU::Vector3f myPosition;
};

// takes in ratio, not actual position
inline EmitterMessage::EmitterMessage(eParticleType aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myParticleType(aParticleType)
	, myEntityID(aEntityID)
{
}

inline EmitterMessage::EmitterMessage(eParticleType aParticleType, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myParticleType(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
{
}