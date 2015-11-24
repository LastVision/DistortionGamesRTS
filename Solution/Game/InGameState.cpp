#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <Engine.h>
#include <FileWatcher.h>
#include <DebugFont.h>
#include <GameStateMessage.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <PostMaster.h>
#include <Sprite.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>

InGameState::InGameState(CU::InputWrapper* anInputWrapper)
{
	myIsActiveState = false;
	myInputWrapper = anInputWrapper;
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy)
{
	myIsLetThrough = false;
	myIsComplete = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

	OnResize(windowSize.x, windowSize.y);
	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);

	myIsActiveState = true;
}

void InGameState::EndState()
{
	
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	return eStateStatus::eKeepState;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);

	VTUNE_EVENT_END();
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
}

void InGameState::OnResize(int aWidth, int aHeight)
{

}

void InGameState::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.GetGameState())
	{
	case eGameState::RELOAD_LEVEL:
		
		break;

	case eGameState::COMPLETE_LEVEL:
		
		break;

	case eGameState::LOAD_NEXT_LEVEL:
		
		break;
	}
}

void InGameState::SetLevel(int aLevelID, int aDifficultID)
{
	
}

void InGameState::CompleteLevel()
{
	
}

void InGameState::CompleteGame()
{
	
	myIsComplete = true;
}

void InGameState::LoadLevelSettings()
{
	
}

void InGameState::LoadPlayerSettings()
{
	
}

void InGameState::ShowMessage(const std::string& aBackgroundPath, 
	const CU::Vector2<float>& aSize, std::string aText, GameStateMessage* aMessage)
{
	
}