#pragma once
#include "Message.h"

struct AITimeMultiplierMessage : public Message
{
	AITimeMultiplierMessage(float aMultiplier);

	const float myMultiplier;
};

// takes in ratio, not actual position
inline AITimeMultiplierMessage::AITimeMultiplierMessage(float aMultiplier)
	: Message(eMessageType::AI_TIME_MULTIPLIER)
	, myMultiplier(aMultiplier)
{
}