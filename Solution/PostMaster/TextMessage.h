#pragma once
#include "Message.h"
#include <string>

struct TextMessage : public Message
{
	TextMessage(const std::string& aText);

	const std::string myText;
};

// takes in ratio, not actual position
inline TextMessage::TextMessage(const std::string& aText)
	: Message(eMessageType::GUI_TEXT)
	, myText(aText)
{
}