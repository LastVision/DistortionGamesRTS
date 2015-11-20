#pragma once

#include "Message.h"

class EventQueueEmptyMessage: public Message
{
public:
	EventQueueEmptyMessage();
};

inline EventQueueEmptyMessage::EventQueueEmptyMessage()
	: Message(eMessageType::EVENT_QUEUE_EMPTY)
{
}