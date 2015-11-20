#pragma once

#include "Message.h"
#include <string>

class Entity;

class DefendMessage : public Message
{
public:
	enum class eType
	{
		NAME,
		ENTITY,
		COMPLETE,
	};

	DefendMessage(eType aType, const std::string& aDefendName = "", Entity* aEntity = nullptr);

	const eType myType;
	const std::string myDefendName;
	Entity* myEntity;
};

