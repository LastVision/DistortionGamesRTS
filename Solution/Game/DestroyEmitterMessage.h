#pragma once
#include "Message.h"

class ParticleEmitterComponent;

class DestroyEmitterMessage : public Message
{
public:
	DestroyEmitterMessage(ParticleEmitterComponent* anParticleEmitterComponent);
	~DestroyEmitterMessage();
	
	ParticleEmitterComponent* myParticleEmitterComponent;

};

