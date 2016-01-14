#pragma once
#include "Message.h"
enum class eTutorialAction
{
	CLICK
};

struct TutorialMessage : public Message
{
	TutorialMessage(eTutorialAction aAction);

	const eTutorialAction myAction;
};

inline TutorialMessage::TutorialMessage(eTutorialAction aAction)
	: Message(eMessageType::TUTORIAL)
	, myAction(aAction)
{
}