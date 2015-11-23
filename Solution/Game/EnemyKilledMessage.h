#pragma once

#include "Message.h"

class EnemyKilledMessage : public Message
{
public:
	EnemyKilledMessage();
};

inline EnemyKilledMessage::EnemyKilledMessage()
	: Message(eMessageType::ENEMY_KILLED)
{
}