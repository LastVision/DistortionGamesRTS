#pragma once
#include "Message.h"
#include <string>

enum class eCinematicAction
{
	START,
	END
};

struct LUACinematicMessage : public Message
{
	LUACinematicMessage(int aCinematicIndex, eCinematicAction aAction);

	const int myCinematicIndex;
	const eCinematicAction myAction;
};

inline LUACinematicMessage::LUACinematicMessage(int aCinematicIndex, eCinematicAction aAction)
	: Message(eMessageType::MOVE_CAMERA)
	, myCinematicIndex(aCinematicIndex)
	, myAction(aAction)
{
}