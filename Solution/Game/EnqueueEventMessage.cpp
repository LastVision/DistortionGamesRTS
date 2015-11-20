#include "stdafx.h"
#include "EnqueueEventMessage.h"


EnqueueEventMessage::EnqueueEventMessage(const std::string& aName)
	: myName(aName)
	, Message(eMessageType::START_EVENT)
{
}

const std::string& EnqueueEventMessage::GetName() const
{
	return myName;
}
