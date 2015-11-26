#pragma once
#include "Message.h"

enum class eOnClickEvent
{
	QUIT,
	RESTART,
	WIN,
	LOSE
};

struct OnClickMessage : public Message
{
	OnClickMessage(eOnClickEvent anEvent);

	const eOnClickEvent myEvent;
};

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
{
}