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
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, bool aShouldAlwaysShow);
	EmitterMessage(const std::string& aParticleType, Prism::Camera* aCamera, const CU::Vector3f& aPosition = CU::Vector3f());
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, const CU::Vector3f& aSize);
	EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, float aRadius);


	const std::string myParticleTypeString;
	const int myEntityID;
	const CU::Vector3f myPosition;
	const float myEmitterLifeTime;
	Prism::Camera* myCamera;
	CU::Vector3f mySize;
	float myRadius;
	bool myShouldAlwaysShow;
};

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, int aEntityID)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myEntityID(aEntityID)
	, myCamera(nullptr)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myCamera(nullptr)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, bool aShouldAlwaysShow)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myCamera(nullptr)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(aShouldAlwaysShow)
{
}


inline EmitterMessage::EmitterMessage(const std::string& aParticleType, Prism::Camera* aCamera, const CU::Vector3f& aPosition)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myCamera(aCamera)
	, myEmitterLifeTime(0.f)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(anEmitterLifeTime)
	, mySize({ 0.f, 0.f, 0.f })
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, float aRadius)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(anEmitterLifeTime)
	, myRadius(aRadius)
	, mySize({ 0.f, 0.f, 0.f })
	, myShouldAlwaysShow(false)
{
}

inline EmitterMessage::EmitterMessage(const std::string& aParticleType, const CU::Vector3f& aPosition, float anEmitterLifeTime, const CU::Vector3f& aSize)
	: Message(eMessageType::PARTICLE)
	, myParticleTypeString(aParticleType)
	, myPosition(aPosition)
	, myEntityID(-1)
	, myEmitterLifeTime(anEmitterLifeTime)
	, mySize(aSize)
	, myRadius(0.f)
	, myShouldAlwaysShow(false)
{
}
