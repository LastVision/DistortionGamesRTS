#include "stdafx.h"
#include "DestroyEmitterMessage.h"

DestroyEmitterMessage::DestroyEmitterMessage(ParticleEmitterComponent* anParticleEmitterComponent)
	: Message(eMessageType::DESTORY_EMITTER)
	, myParticleEmitterComponent(anParticleEmitterComponent)
{
}


DestroyEmitterMessage::~DestroyEmitterMessage()
{
}
