#pragma once
#include "Message.h"
#include "Vector.h"

class Entity;


struct TriggerMessage : public Message
{
	enum class eTriggerType
	{
		ENTER,
		EXIT,
	};
	TriggerMessage(Entity* aTrigger, Entity* aUnit, eTriggerType aType);

	const eTriggerType myType;
	const CU::Vector2<float> myPosition;
	const Entity* myTrigger;
	const Entity* myUnit;
};

inline TriggerMessage::TriggerMessage(Entity* aTrigger, Entity* aUnit, eTriggerType aType)
	: Message(eMessageType::TRIGGER)
	, myTrigger(aTrigger)
	, myUnit(aUnit)
	, myType(aType)
{
}