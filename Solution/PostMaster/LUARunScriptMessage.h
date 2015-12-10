#pragma once
#include "Message.h"
#include <string>

struct LUARunScriptMessage : public Message
{
	LUARunScriptMessage(const std::string& aFile);

	const std::string myFilePath;
};

inline LUARunScriptMessage::LUARunScriptMessage(const std::string& aFile)
	: Message(eMessageType::LUA_RUN_SCRIPT)
	, myFilePath(aFile)
{
}