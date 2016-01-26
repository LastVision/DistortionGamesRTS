#pragma once
#include "Message.h"
#include <GameEnum.h>
#include <Vector.h>

namespace Prism
{
	class Camera;
}

struct EmitterMessage : public Message
{
	EmitterMessage(const std::string& aParticleType, int aEntityID);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition);
	EmitterMessage(const std::string& aParticleType, Prism::Camera* aCamera, const CU::Vector3f& aPosition = CU::Vector3f());

	const eParticleType myParticleType;
	const std::string myParticleTypeString;
	const int myEntityID;
	const CU::Vector3f myPosition;
	Prism::Camera* myCamera;
};

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myParticleType(eParticleType::NONE)
	, myParticleTypeString(aParticleType)
	, myEntityID(aEntityID)
	, myCamera(nullptr)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myParticleType(eParticleType::NONE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myCamera(nullptr)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, Prism::Camera* aCamera, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myParticleType(eParticleType::NONE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myCamera(aCamera)
{

}