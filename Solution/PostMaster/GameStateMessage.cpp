#include "stdafx.h"
#include "GameStateMessage.h"

GameStateMessage::GameStateMessage(eGameState aGameState)
	: myGameState(aGameState)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const std::string& aFilePath)
	: myGameState(aGameState)
	, myFilePath(aFilePath)
	, myID(-1)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const std::string& aFilePath, const int& aID)
	: myGameState(aGameState)
	, myFilePath(aFilePath)
	, myID(aID)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const int& anID)
	: myGameState(aGameState)
	, myID(anID)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const int& anID, const int& anSecondID)
	: myGameState(aGameState)
	, myID(anID)
	, mySecondID(anSecondID)
	, Message(eMessageType::GAME_STATE)
{
}

GameStateMessage::GameStateMessage(eGameState aGameState, const bool& anIsMouseLocked)
	: myGameState(aGameState)
	, myMouseIsLocked(anIsMouseLocked)
	, Message(eMessageType::GAME_STATE)
{
}
