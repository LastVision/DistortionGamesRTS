#pragma once
#include "Message.h"

struct EmitterMessage : public Message
{
	EmitterMessage(float aDuration);

	const float myDuration;
};

// takes in ratio, not actual position
inline EmitterMessage::EmitterMessage(float aDuration)
	: Message(eMessageType::FADE)
	, myDuration(aDuration)
{
}