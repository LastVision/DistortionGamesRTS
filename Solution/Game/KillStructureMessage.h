#pragma once

#include "Message.h"
#include <string>

class Entity;

class KillStructureMessage : public Message
{
public:
	enum class eType
	{
		NAME,
		ENTITY,
		TO_GUI
	};

	KillStructureMessage(eType aType, const std::string& aStructureName = "", Entity* aEntity = nullptr);

	const eType myType;
	const std::string myStructureName;
	Entity* myEntity;
};

