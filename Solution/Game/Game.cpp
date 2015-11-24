#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <CommonHelper.h>
#include <Cursor.h>
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

#include <Sprite.h>

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

	myGUIManager = new GUI::GUIManager(myInputWrapper);
	myCursor = new GUI::Cursor(myInputWrapper, Prism::Engine::GetInstance()->GetWindowSize());

	SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
}

Game::~Game()
{
	delete mySprite;
	delete myInputWrapper;
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


	mySprite = new Prism::Sprite("Data/Resource/Texture/Star.dds", { 128.f, 128.f }, { 64.f, 64.f });
	GAME_LOG("Init Successful");
	return true;
}

bool Game::Destroy()
{
	return true;
}

bool Game::Update()
{
	mySprite->Render({ 0, 100 });
	myInputWrapper->Update();
	if (myInputWrapper->KeyDown(DIK_ESCAPE))
	{
		return false;
	}

	if (myLockMouse == true)
	{
		SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
	}

	myGUIManager->Update();
	myGUIManager->Render();

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
}
