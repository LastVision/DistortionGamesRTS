#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <CommonHelper.h>
#include <Engine.h>
#include <FileWatcher.h>
#include <DebugFont.h>
#include "Game.h"
#include <GUIManager.h>
#include <InputWrapper.h>
#include <ModelLoader.h>
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
	Prism::Audio::AudioInterface::CreateInstance();
	myInputWrapper = new CU::InputWrapper();
	Prism::Engine::GetInstance()->SetShowDebugText(myShowSystemInfo);
}

Game::~Game()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myInputWrapper);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MenuMusic", 0);
	Prism::Audio::AudioInterface::Destroy();
}

bool Game::Init(HWND& aHwnd)
{
	myWindowHandler = &aHwnd;
	myIsComplete = false;

	Prism::Engine::GetInstance()->SetClearColor({ MAGENTA });
	myInputWrapper->Init(aHwnd, GetModuleHandle(NULL), DISCL_NONEXCLUSIVE 
		| DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	myWindowSize.x = Prism::Engine::GetInstance()->GetWindowSize().x;
	myWindowSize.y = Prism::Engine::GetInstance()->GetWindowSize().y;

	myGUIManager = new GUI::GUIManager(myInputWrapper);

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
	if (myInputWrapper->KeyDown(DIK_ESCAPE))
	{
		return false;
	}

	myGUIManager->Update();
	myGUIManager->Render();

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
}
