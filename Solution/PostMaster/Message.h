#pragma once

#include "MessageEnum.h"

class Message
{
public:
	Message(eMessageType aMessageType);
	eMessageType GetMessageType() const;

protected:
	void operator=(Message&) = delete;
	const eMessageType myMessageType;
};

inline Message::Message(eMessageType aMessageType)
	:myMessageType(aMessageType)
{
}

inline eMessageType Message::GetMessageType() const
{
	return myMessageType;
}