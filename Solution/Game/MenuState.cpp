#include "stdafx.h"

#include <AudioInterface.h>
#include "Button.h"
#include <Camera.h>
#include <Engine.h>
#include "FadeMessage.h"
#include "GameStateMessage.h"
#include <InputWrapper.h>
#include "Menu.h"
#include "MenuState.h"
#include <ModelLoader.h>
#include <Sprite.h>
#include "SplashState.h"
#include "StateStackProxy.h"
#include <TimerManager.h>
#include "PostMaster.h"
#include <XMLReader.h>

MenuState::MenuState(const std::string& aXMLPath, CU::InputWrapper* anInputWrapper, int aLevelID, bool aShowVictoryScreen)
	: myHasFadeIn(aShowVictoryScreen)
	, myShowVictoryScreen(aShowVictoryScreen)
	, myShowButtons(!aShowVictoryScreen)
	, myWaitForButtonsTime(4.f)
{
	Prism::Engine::GetInstance()->GetModelLoader()->Pause();

	myInputWrapper = anInputWrapper;
	
	if (aLevelID == -1)
	{
		myMenu = new Menu(aXMLPath);
	}
	else
	{
		myMenu = new Menu(aXMLPath, aLevelID);
	}

	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));

	CU::Vector2<float> overlaySize(4096.f, 4096.f);
	myBlackOverlay = new Prism::Sprite("Data/Resource/Texture/Menu/Splash/T_background_default.dds", windowSize, windowSize / 2.f);

	Prism::Engine::GetInstance()->GetModelLoader()->UnPause();
}

MenuState::~MenuState()
{
}

void MenuState::InitState(StateStackProxy* aStateStackProxy)
{
	Prism::Engine::GetInstance()->myIsLoading = true;
	Prism::Engine::GetInstance()->GetModelLoader()->Pause();

	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	CU::Matrix44<float> orientation;
	myCamera = new Prism::Camera(orientation);
	OnResize(Prism::Engine::GetInstance()->GetWindowSize().x, Prism::Engine::GetInstance()->GetWindowSize().y);
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::MOUSE_LOCK, true));

	if (myMenu->GetMainMenu() == true)
	{
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_other.dds", myInputWrapper));
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_our.dds", myInputWrapper));
		myHasFadeIn = true;
	}

	if (myShowVictoryScreen == true)
	{
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_victory_screen.dds", myInputWrapper, true));
	}

	myCurrentTime = 0;
	myFadeInTime = 0.5f;
	myOverlayAlpha = 1.f;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
	Prism::Engine::GetInstance()->GetModelLoader()->UnPause();
}

void MenuState::EndState()
{
	delete myMenu;
	delete myCamera;
	myMenu = nullptr;
	myCamera = nullptr;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

const eStateStatus MenuState::Update(const float& aDeltaTime)
{
	if (myShowButtons == false)
	{
		myWaitForButtonsTime -= aDeltaTime;
		if (myWaitForButtonsTime <= 0.f)
		{
			myShowButtons = true;
			myWaitForButtonsTime = 5.f;
			PostMaster::GetInstance()->SendMessage(FadeMessage(0.33f));
		}
	}

	if (myInputWrapper->KeyDown(DIK_ESCAPE) == true)
	{
		return eStateStatus::ePopMainState;
	}

	myCurrentTime += aDeltaTime;
	myOverlayAlpha = fmaxf(1.f - myCurrentTime / myFadeInTime, 0);

	if (myMenu->Update(myInputWrapper, myShowButtons) == false)
	{
		return eStateStatus::ePopMainState;
	}

	return eStateStatus::eKeepState;
}

void MenuState::Render()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(-Prism::Engine::GetInstance()->GetWindowSize().y));

	if (myHasFadeIn == true)
	{
		myMenu->Render(myInputWrapper, myShowButtons);
		myBlackOverlay->Render(windowSize / 2.f, { 1.f, 1.f }, { 1.f, 1.f, 1.f, myOverlayAlpha });
	}
	else
	{
		myBlackOverlay->Render(windowSize / 2.f, { 1.f, 1.f });
		myMenu->Render(myInputWrapper, myShowButtons);
	}
}

void MenuState::ResumeState()
{
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::MOUSE_LOCK, true));
	myMenu->Reload();
}

void MenuState::OnResize(int aWidth, int aHeight)
{
	myCamera->OnResize(aWidth, aHeight);
	myMenu->OnResize(aWidth, aHeight);
}