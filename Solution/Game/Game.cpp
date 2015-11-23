#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <CommonHelper.h>
#include <Engine.h>
#include <FileWatcher.h>
#include <DebugFont.h>
#include "Game.h"
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
	delete myInputWrapper;
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MenuMusic", 0);
	Prism::Audio::AudioInterface::Destroy();
}

bool Game::Init(HWND& aHwnd)
{
	myWindowHandler = &aHwnd;
	myIsComplete = false;


	myInputWrapper->Init(aHwnd, GetModuleHandle(NULL)
		, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);


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

	if (myInputWrapper->KeyDown(DIK_ESCAPE))
	{
		return false;
	}

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