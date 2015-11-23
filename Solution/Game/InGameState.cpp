#include "stdafx.h"

#include <AudioInterface.h>
#include "BulletManager.h"
#include <Camera.h>
#include "CollisionManager.h"
#include "ColoursForBG.h"
#include "Constants.h"
#include <Engine.h>
#include "Entity.h"
#include "FadeMessage.h"
#include <FileWatcher.h>
#include <DebugFont.h>
#include "GameStateMessage.h"
#include "InGameState.h"
#include "InGameMenuState.h"
#include <InputWrapper.h>
#include "Level.h"
#include "LevelFactory.h"
#include "LoadingScreen.h"
#include "MessageState.h"
#include <ModelLoader.h>
#include "PostMaster.h"
#include <Sprite.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>

InGameState::InGameState(CU::InputWrapper* anInputWrapper)
	: myLoadingScreen(nullptr)
{
	myIsActiveState = false;
	myInputWrapper = anInputWrapper;
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Pause_MenuMusic", 0);
}

InGameState::~InGameState()
{
	delete myLevelFactory;
	delete myLevel;
	delete myLoadingScreen;
	myLevelFactory = nullptr;
	myLevel = nullptr;
	myLoadingScreen = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Resume_MenuMusic", 0);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy)
{
	myLevelFactory = new LevelFactory("Data/Level/LI_list_level.xml", myInputWrapper);
	myIsLetThrough = false;
	myIsComplete = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

	OnResize(windowSize.x, windowSize.y);
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::MOUSE_LOCK, true));
	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
	myLoadingScreen = new LoadingScreen(myInputWrapper, myLevelFactory->IsLevelLoading(), myLevelFactory->GetLevelID());

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));

	myIsActiveState = true;
}

void InGameState::EndState()
{
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	if (myLoadingScreen->IsDone() == true)
	{
		if (myLevelFactory->IsClean() == false)
		{
			myLevelFactory->Cleanup();
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f/3.f));
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("UnMute", 0);
		}

		if (myIsComplete == true)
		{
			PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::COMPLETE_GAME));
			return eStateStatus::ePopMainState;
		} 

		if (myInputWrapper->KeyDown(DIK_ESCAPE))
		{
			myIsActiveState = false;
			myStateStack->PushSubGameState(new InGameMenuState("Data/Menu/MN_ingame_menu.xml", myInputWrapper));
		}

		if (myLevel->LogicUpdate(aDeltaTime) == true)
		{
			GameStateMessage* newEvent = new GameStateMessage(eGameState::RELOAD_LEVEL);
			ShowMessage("Data/Resource/Texture/Menu/T_background_game_over.dds", { 1024, 1024 }, "Press [space] to restart.", newEvent);
			return eStateStatus::eKeepState;
		}
	}
	else
	{
		myLoadingScreen->Update(aDeltaTime);
		if (myLoadingScreen->IsDone() == true)
		{
			//Need to call Update again to fix Cockpit and Skysphere on the first frame
			Update(aDeltaTime);
		}
	}
	return eStateStatus::eKeepState;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);

	if (myLoadingScreen->IsDone() == true)
	{
		myLevel->Render(myIsActiveState);
	}
	else
	{
		myLoadingScreen->Render();
	}

	VTUNE_EVENT_END();
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::MOUSE_LOCK, true));
}

void InGameState::OnResize(int aWidth, int aHeight)
{
	if (myLevel != nullptr)
	{
		myLevel->OnResize(aWidth, aHeight);
	}

	if (myLoadingScreen != nullptr)
	{
		myLoadingScreen->OnResize(aWidth, aHeight);
	}
}

void InGameState::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.GetGameState())
	{
	case eGameState::RELOAD_LEVEL:
		myLoadingScreen->Reset();
		myLevel = myLevelFactory->LoadCurrentLevel();
		break;

	case eGameState::COMPLETE_LEVEL:
		if (myLevelFactory->IsLastLevel() == false)
		{
			CompleteLevel();
		}
		else
		{
			CompleteGame();
		}
		break;

	case eGameState::LOAD_NEXT_LEVEL:
		myLoadingScreen->Reset();
		myLevel = myLevelFactory->LoadNextLevel();
		break;
	}
}

void InGameState::SetLevel(int aLevelID, int aDifficultID)
{
	myLoadingScreen->Reset();
	myLevel = myLevelFactory->LoadLevel(aLevelID, aDifficultID);
}

void InGameState::CompleteLevel()
{
	GameStateMessage* newEvent = new GameStateMessage(eGameState::LOAD_NEXT_LEVEL);
	ShowMessage("Data/Resource/Texture/Menu/T_background_completed_level.dds", { 1024, 1024 }, "Press [space] to continue.", newEvent);
}

void InGameState::CompleteGame()
{
	ShowMessage("Data/Resource/Texture/Menu/T_background_completed_game.dds", { 1024, 1024 }, "Press [space] to continue.");
	myIsComplete = true;
}

void InGameState::LoadLevelSettings()
{
	myLevelFactory->ReadLevelSettings();
}

void InGameState::LoadPlayerSettings()
{
	myLevelFactory->LoadPlayer();
}

void InGameState::ShowMessage(const std::string& aBackgroundPath, 
	const CU::Vector2<float>& aSize, std::string aText, GameStateMessage* aMessage)
{
	myIsActiveState = false;
	LevelScore score = myLevel->GetLevelScore();
	myMessageScreen = new MessageState(aBackgroundPath, aSize, myInputWrapper, score);
	myMessageScreen->SetText(aText);
	myMessageScreen->SetEvent(aMessage);
	myStateStack->PushSubGameState(myMessageScreen);
}