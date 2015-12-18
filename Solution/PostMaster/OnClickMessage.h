#pragma once
#include "Message.h"

enum class eOnClickEvent
{
	GAME_START,
	GAME_QUIT,
	GAME_RESTART,
	GAME_WIN,
	GAME_LOSE,
	SPAWN_UNIT,
	UNIT_ACTION_MOVE,
	UNIT_ACTION_ATTACK,
	UNIT_ACTION_STOP,
	UNIT_ACTION_ATTACK_MOVE,
	UNIT_ACTION_PATROL,
	UNIT_ACTION_STAND_GROUND,
	PLACE_TOTEM,
	ENRAGE
};

struct OnClickMessage : public Message
{
	OnClickMessage(eOnClickEvent anEvent);
	OnClickMessage(eOnClickEvent anEvent, int anID);

	const eOnClickEvent myEvent;
	const int myID;
};

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(-1)
{
}

inline OnClickMessage::OnClickMessage(eOnClickEvent anEvent, int anID)
	: Message(eMessageType::ON_CLICK)
	, myEvent(anEvent)
	, myID(anID)
{

}