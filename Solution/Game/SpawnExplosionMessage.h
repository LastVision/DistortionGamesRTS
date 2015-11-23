#pragma once
#include "Message.h"

class SpawnExplosionMessage : public Message
{
public:
	SpawnExplosionMessage(eMessageType anExplosionType, CU::Vector3f aPosition);
	~SpawnExplosionMessage();


	CU::Vector3f myPosition;
	
};



