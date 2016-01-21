#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <Vector.h>

struct EmitterMessage : public Message
{
	EmitterMessage(const std::string& aParticleType, int aEntityID);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition);


	const eParticleType myParticleType;
	const std::string myParticleTypeString;
	const int myEntityID;
	const CU::Vector3f myPosition;
};

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myParticleType(eParticleType::NONE)
	, myParticleTypeString(aParticleType)
	, myEntityID(aEntityID)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myParticleType(eParticleType::NONE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
{
}