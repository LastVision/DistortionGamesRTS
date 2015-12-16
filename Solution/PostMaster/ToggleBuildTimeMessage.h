#pragma once
#include "Message.h"

struct ToggleBuildTimeMessage : public Message
{
	ToggleBuildTimeMessage(bool aUseBuildTime);

	const bool myUseBuildTime;
};

// takes in ratio, not actual position
inline ToggleBuildTimeMessage::ToggleBuildTimeMessage(bool aUseBuildTime)
	: Message(eMessageType::TOGGLE_BUILD_TIME)
	, myUseBuildTime(aUseBuildTime)
{
}