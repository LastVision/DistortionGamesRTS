#pragma once

#include "Message.h"

class EnqueueEventMessage : public Message
{
public:
	EnqueueEventMessage(const std::string& aName);

	const std::string& GetName() const;

private:
	const std::string myName;
};

