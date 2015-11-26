#pragma once
#include "Message.h"

namespace GUI
{
	enum class eOnClickEvent
	{
		QUIT,
		RESTART,
		WIN,
		LOSE
	};
}

struct OnClickMessage : public Message
{
	OnClickMessage(GUI::eOnClickEvent aEvent);

	const GUI::eOnClickEvent myEvent;
};

OnClickMessage::OnClickMessage(GUI::eOnClickEvent aEvent)
	: Message(eMessageType::ON_CLICK)
	, myEvent(aEvent)
{
}