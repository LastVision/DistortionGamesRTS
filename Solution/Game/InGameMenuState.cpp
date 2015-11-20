#include "stdafx.h"
#include "InGameMenuState.h"
#include <Sprite.h>
#include <Camera.h>
#include "FadeMessage.h"
#include <InputWrapper.h>
#include "SplashState.h"
#include "StateStackProxy.h"
#include <TimerManager.h>
#include "PostMaster.h"
#include "GameStateMessage.h"
#include "Menu.h"
#include "Button.h"

InGameMenuState::InGameMenuState(const std::string& aXMLPath, CU::InputWrapper* anInputWrapper, bool anIsIngame)
{
	myInputWrapper = anInputWrapper;
	myIsInGame = anIsIngame;

	myMenu = new Menu(aXMLPath);

	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));

	CU::Vector2<float> overlaySize(4096.f, 4096.f);
	myBlackOverlay = new Prism::Sprite("Data/Resource/Texture/Menu/Splash/T_background_default.dds", windowSize, windowSize / 2.f);
}

InGameMenuState::~InGameMenuState()
{
}

void InGameMenuState::InitState(StateStackProxy* aStateStackProxy)
{
	myIsLetThrough = true;
	myStateStack = aStateStackProxy;
	CU::Matrix44<float> orientation;
	myCamera = new Prism::Camera(orientation);
	OnResize(Prism::Engine::GetInstance()->GetWindowSize().x, Prism::Engine::GetInstance()->GetWindowSize().y);
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::MOUSE_LOCK, false));

	myCurrentTime = 0;
	myFadeInTime = 0.5f;
	myOverlayAlpha = 1.f;

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f/3.f));
}

void InGameMenuState::EndState()
{
	delete myMenu;
	delete myCamera;
	myMenu = nullptr;
	myCamera = nullptr;

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

const eStateStatus InGameMenuState::Update(const float& aDeltaTime)
{
	aDeltaTime;
	if (myInputWrapper->KeyDown(DIK_ESCAPE) == true)
	{
		return eStateStatus::ePopSubState;
	}
#ifndef RELEASE_BUILD
	if (myInputWrapper->KeyDown(DIK_SPACE) == true)
	{
		return eStateStatus::ePopMainState;
	}
#endif
	float deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
	deltaTime = fminf(1.f / 30.f, deltaTime);

	myCurrentTime += deltaTime;
	myOverlayAlpha = fmaxf(1.f - myCurrentTime / myFadeInTime, 0);

	return myMenu->Update(myInputWrapper, true, !myIsInGame);
}

void InGameMenuState::Render()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(-Prism::Engine::GetInstance()->GetWindowSize().y));

	myBlackOverlay->Render(windowSize / 2.f, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 0.5f });

	myMenu->Render(myInputWrapper, true, !myIsInGame);

	if (myMenu->GetMainMenu() == true)
	{
		myBlackOverlay->Render(windowSize / 2.f, { 1.f, 1.f }, { 1.f, 1.f, 1.f, myOverlayAlpha });
	}
}

void InGameMenuState::ResumeState()
{
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::MOUSE_LOCK, false));
}

void InGameMenuState::OnResize(int aWidth, int aHeight)
{
	myCamera->OnResize(aWidth, aHeight);
	myMenu->OnResize(aWidth, aHeight);
}