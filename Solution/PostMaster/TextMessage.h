#pragma once
#include "Message.h"

namespace Prism
{
	class Text;
}

struct TextMessage : public Message
{
	TextMessage(Prism::Text* aText);

	Prism::Text* myText;
};

inline TextMessage::TextMessage(Prism::Text* aText)
	: Message(eMessageType::TEXT)
	, myText(aText)
{
}