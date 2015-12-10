#pragma once
#include "Message.h"

struct LUAToggleRenderLinesMessage : public Message
{
	LUAToggleRenderLinesMessage(bool aToggleFlag);

	const bool myToggleFlag;
};

inline LUAToggleRenderLinesMessage::LUAToggleRenderLinesMessage(bool aToggleFlag)
	: Message(eMessageType::TOGGLE_LINES)
	, myToggleFlag(aToggleFlag)
{
}