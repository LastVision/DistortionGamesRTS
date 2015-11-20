#include "stdafx.h"

#include <AudioInterface.h>
#include "BulletManager.h"
#include <Camera.h>
#include "ColoursForBG.h"
#include <CommonHelper.h>
#include "Constants.h"
#include <Engine.h>
#include "FadeMessage.h"
#include <FileWatcher.h>
#include <DebugFont.h>
#include "Game.h"
#include "GameStateMessage.h"
#include "InGameState.h"
#include "InGameMenuState.h"
#include <InputWrapper.h>
#include "Level.h"
#include "LevelFactory.h"
#include "MenuState.h"
#include <ModelLoader.h>
#include "PostMaster.h"
#include "ResizeMessage.h"
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

	
}

Game::~Game()
{
	delete myInputWrapper;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::FADE, this);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MenuMusic", 0);
	Prism::Audio::AudioInterface::Destroy();
	PostMaster::Destroy();
}

bool Game::Init(HWND& aHwnd)
{
	myWindowHandler = &aHwnd;
	myIsComplete = false;
	bool startInMenu = false;

	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_options.xml");
	reader.ReadAttribute(reader.FindFirstChild("startInMenu"), "bool", startInMenu);
#ifdef _DEBUG
	//startInMenu = false;
#endif

	startInMenu = true;

	reader.CloseDocument();
	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::FADE, this);

	Prism::Audio::AudioInterface::GetInstance()->Init("Data/Resource/Sound/Init.bnk");
	Prism::Audio::AudioInterface::GetInstance()->LoadBank("Data/Resource/Sound/SpaceShooterBank.bnk");

	myInputWrapper->Init(aHwnd, GetModuleHandle(NULL)
		, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	if (startInMenu == false)
	{
		PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::LOAD_GAME, 1));
	}
	else
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_MenuMusic", 0);
		myCurrentMenu = new MenuState("Data/Menu/MN_main_menu.xml", myInputWrapper);
		myStateStack.PushMainGameState(myCurrentMenu);
		myLockMouse = false;
	}

	Prism::Engine::GetInstance()->SetClearColor({ MAGENTA });

	myWindowSize.x = Prism::Engine::GetInstance()->GetWindowSize().x;
	myWindowSize.y = Prism::Engine::GetInstance()->GetWindowSize().y;


	GAME_LOG("Init Successful");
	return true;
}

bool Game::Destroy()
{
	return true;
}

bool Game::Update()
{
	Prism::Audio::AudioInterface::GetInstance()->Update();
	myInputWrapper->Update();
	CU::TimerManager::GetInstance()->Update();

	float deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
	float realDeltaTime = deltaTime;
	if (deltaTime > 1.0f / 10.0f)
	{
		deltaTime = 1.0f / 10.0f;
	}

#ifndef RELEASE_BUILD
	if (myInputWrapper->KeyUp(DIK_O) == true)
	{
		myLockMouse = !myLockMouse;
		ShowCursor(!myLockMouse);
	}
#endif

	if (myLockMouse == true)
	{
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
		//SetCursorPos(myWindowSize.x / 2, myWindowSize.y / 2);
	}

	if (myStateStack.UpdateCurrentState(deltaTime) == false)
	{
		return false;
	}

#ifndef RELEASE_BUILD
	if (myInputWrapper->KeyDown(DIK_F8))
	{
		myShowSystemInfo = !myShowSystemInfo;
		Prism::Engine::GetInstance()->SetShowDebugText(myShowSystemInfo);
	}
	if (myInputWrapper->KeyDown(DIK_F9))
	{
		Prism::Engine::GetInstance()->GetFileWatcher()->CheckFiles();
	}
	if (myInputWrapper->KeyDown(DIK_F3))
	{
		PostMaster::GetInstance()->SendMessage(FadeMessage(1.f/3.f));
	}
#endif

	myStateStack.RenderCurrentState();

#ifndef RELEASE_BUILD
	if (myShowSystemInfo == true)
	{
		int fps = int(1.f / realDeltaTime);
		float frameTime = realDeltaTime * 1000.f;
		int memory = Prism::SystemMonitor::GetMemoryUsageMB();
		float cpuUsage = Prism::SystemMonitor::GetCPUUsage();

		Prism::Engine::GetInstance()->PrintText(CU::Concatenate("FPS: %d", fps), { 1000.f, -20.f }, Prism::eTextType::DEBUG_TEXT);
		Prism::Engine::GetInstance()->PrintText(CU::Concatenate("FrameTime: %f", frameTime), { 1000.f, -50.f }, Prism::eTextType::DEBUG_TEXT);
		Prism::Engine::GetInstance()->PrintText(CU::Concatenate("Mem: %d (MB)", memory), { 1000.f, -80.f }, Prism::eTextType::DEBUG_TEXT);
		Prism::Engine::GetInstance()->PrintText(CU::Concatenate("CPU: %f", cpuUsage), { 1000.f, -110.f }, Prism::eTextType::DEBUG_TEXT);
	}
#endif

	if (myIsComplete == true)
	{
		myCurrentMenu = new MenuState("Data/Menu/MN_credits.xml", myInputWrapper, -1, true);
		myStateStack.PushMainGameState(myCurrentMenu);
		myIsComplete = false;
	}

	//cap framerate
	CU::TimerManager::GetInstance()->CapFrameRate(100.f);

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

void Game::ReceiveMessage(const FadeMessage& aMessage)
{
	Prism::Engine::GetInstance()->StartFade(aMessage.myDuration);
}

void Game::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.GetGameState())
	{
	case eGameState::LOAD_GAME:
		myGame = new InGameState(myInputWrapper);
		myStateStack.PushSubGameState(myGame);
		myGame->SetLevel(aMessage.GetID(), aMessage.GetSecondID());
		break;
	case eGameState::LOAD_MENU:
		if (aMessage.GetID() == -1)
		{
			myCurrentMenu = new MenuState(aMessage.GetFilePath(), myInputWrapper);
		}
		else if (aMessage.GetID() == -2)
		{
			myStateStack.PushSubGameState(new InGameMenuState("Data/Menu/MN_ingame_menu.xml", myInputWrapper, false));
			return;
		}
		else
		{
			myCurrentMenu = new MenuState(aMessage.GetFilePath(), myInputWrapper, aMessage.GetID());
		}
		myStateStack.PushMainGameState(myCurrentMenu);
		break;
	case eGameState::MOUSE_LOCK:
		myLockMouse = aMessage.GetMouseLocked();
		ShowCursor(!myLockMouse);
		break;
	case eGameState::COMPLETE_GAME:
		myIsComplete = true;
		break;
	}
}