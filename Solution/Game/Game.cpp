#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <CommonHelper.h>
#include <Cursor.h>
#include <DebugFont.h>
#include <Engine.h>
#include <FileWatcher.h>
#include "Game.h"
#include <GameStateMessage.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <ParticleDataContainer.h>
#include <PostMaster.h>
#include <ResizeMessage.h>
#include <SystemMonitor.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>
#include <XMLReader.h>

Game::Game()
	: myLockMouse(true)
#ifdef RELEASE_BUILD
	, myShowSystemInfo(false)
#else
	, myShowSystemInfo(true)
#endif
{
	PostMaster::Create();
	Prism::Audio::AudioInterface::CreateInstance();
	myInputWrapper = new CU::InputWrapper();
	Prism::Engine::GetInstance()->SetShowDebugText(myShowSystemInfo);

	myCursor = new GUI::Cursor(myInputWrapper, Prism::Engine::GetInstance()->GetWindowSize());
	myGUIManager = new GUI::GUIManager(myInputWrapper, myCursor);

	SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
}

Game::~Game()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myInputWrapper);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	SAFE_DELETE(myCursor);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MenuMusic", 0);
	Prism::Audio::AudioInterface::Destroy();
	Prism::ParticleDataContainer::Destroy();
	PostMaster::Destroy();
	myStateStack.Clear();
}

bool Game::Init(HWND& aHwnd)
{
	myWindowHandler = &aHwnd;
	myIsComplete = false;

	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);

	Prism::Engine::GetInstance()->SetClearColor({ MAGENTA });
	myInputWrapper->Init(aHwnd, GetModuleHandle(NULL), DISCL_NONEXCLUSIVE 
		| DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	myWindowSize.x = Prism::Engine::GetInstance()->GetWindowSize().x;
	myWindowSize.y = Prism::Engine::GetInstance()->GetWindowSize().y;

	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::LOAD_GAME, 1));

	GAME_LOG("Init Successful");
	return true;
}

bool Game::Destroy()
{
	return true;
}

bool Game::Update()
{
	myInputWrapper->Update();
	CU::TimerManager::GetInstance()->Update();
	float deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
	float realDeltaTime = deltaTime;
	if (deltaTime > 1.0f / 10.0f)
	{
		deltaTime = 1.0f / 10.0f;
	}

	if (myLockMouse == true)
	{
		//SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
	
		RECT windowRect;
		GetWindowRect(*myWindowHandler, &windowRect);
		if (Prism::Engine::GetInstance()->IsFullscreen() == false)
		{
			windowRect.left += 10;
			windowRect.top += 35;
			windowRect.right -= 10;
			windowRect.bottom -= 10;
		}
		ClipCursor(&windowRect);
	}

	myGUIManager->Update();
	myGUIManager->Render();

	if (myStateStack.UpdateCurrentState(deltaTime) == false)
	{
		return false;
	}

	myStateStack.RenderCurrentState();

	CU::TimerManager::GetInstance()->CapFrameRate(100.f);
	myCursor->Update();
	myCursor->Render();

	return true;
}

void Game::Pause()
{
	myLockMouse = false;
	ShowCursor(true);
}

void Game::UnPause()
{
	myLockMouse = true;
	ShowCursor(false);	
}

void Game::OnResize(int aWidth, int aHeight)
{
	myWindowSize.x = aWidth;
	myWindowSize.y = aHeight;
	myStateStack.OnResize(aWidth, aHeight);
	PostMaster::GetInstance()->SendMessage(ResizeMessage(aWidth, aHeight));
}

void Game::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.GetGameState())
	{
	case eGameState::LOAD_GAME:
		myGame = new InGameState(myInputWrapper);
		myStateStack.PushMainGameState(myGame);
		myGame->SetLevel(aMessage.GetID(), aMessage.GetSecondID());
		break;
	case eGameState::LOAD_MENU:
		break;
	default:
		break;
	}
}
