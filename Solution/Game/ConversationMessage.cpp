#include "stdafx.h"
#include "ConversationMessage.h"


ConversationMessage::ConversationMessage(const std::string& aText)
	: myText(aText)
	, Message(eMessageType::CONVERSATION)
{
}