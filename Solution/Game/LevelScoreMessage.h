#pragma once
#include "Message.h"
class LevelScoreMessage : public Message
{
public:
	LevelScoreMessage(const eLevelScoreMessageType& aScoreMessageType);

	const eLevelScoreMessageType myScoreMessageType;
};

