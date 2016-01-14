#include "stdafx.h"
#include "GameStateMessage.h"

GameStateMessage::GameStateMessage(eGameState aGameState)
	: myGameState(aGameState)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const std::string& aFilePath)
	: myGameState(aGameState)
	, myFilePath(aFilePath)
	, myID(-1)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const std::string& aFilePath, const int& aID)
	: myGameState(aGameState)
	, myFilePath(aFilePath)
	, myID(aID)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const float& aTime, Prism::Text* aText)
	: myGameState(aGameState)
	, myTime(aTime)
	, myText(aText)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const int& anID)
	: myGameState(aGameState)
	, myID(anID)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const int& anID, const int& anSecondID)
	: myGameState(aGameState)
	, myID(anID)
	, mySecondID(anSecondID)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const bool& anIsMouseLocked)
	: myGameState(aGameState)
	, myMouseIsLocked(anIsMouseLocked)
	, myText(nullptr)
	, Message(eMessageType::GAME_STATE)
{
}
