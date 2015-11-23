#pragma once

#include "Message.h"
#include <string>

class ConversationMessage : public Message
{
public:
	ConversationMessage(const std::string& aText);

	const std::string myText;
};

