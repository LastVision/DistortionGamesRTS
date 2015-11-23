#include "stdafx.h"
#include "LevelScoreMessage.h"


LevelScoreMessage::LevelScoreMessage(const eLevelScoreMessageType& aScoreMessageType)
	: Message(eMessageType::LEVEL_SCORE)
	, myScoreMessageType(aScoreMessageType)
{
}