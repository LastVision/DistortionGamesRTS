#pragma once
#include "Message.h"

class FadeMessage : public Message
{
public:
	FadeMessage(float aDuration);
	
	const float myDuration;
};

