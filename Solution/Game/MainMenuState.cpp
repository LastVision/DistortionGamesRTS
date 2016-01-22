#include "stdafx.h"
#include <Camera.h>
#include "CreditMenuState.h"
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "LevelSelectState.h"
#include "MainMenuState.h"
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "SplashState.h"
#include "StateStackProxy.h"

MainMenuState::MainMenuState()
{
}

MainMenuState::~MainMenuState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void MainMenuState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_main_menu.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	myHasRunOnce = false;
}

void MainMenuState::EndState()
{

}

const eStateStatus MainMenuState::Update(const float& aDeltaTime)
{
	if (myHasRunOnce == false)
	{
#ifdef RELEASE_BUILD
		PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
		bool runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
		Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_other.dds", false));
		Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
		runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
		Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_our.dds", true));
		Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
#endif
		myHasRunOnce = true;
	}
	else 
	{
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
		{
			myIsActiveState = false;
			return eStateStatus::ePopMainState;
		}

		myGUIManager->Update(aDeltaTime);
	}
	return myStateStatus;
}

void MainMenuState::Render()
{
	myGUIManager->Render();
}

void MainMenuState::ResumeState()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
}

void MainMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void MainMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::GAME_START:
			Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushMainGameState(new InGameState(aMessage.myID));
			break;
		case eOnClickEvent::GAME_LEVEL_SELECT:
			Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushMainGameState(new LevelSelectState());
			break;
		case eOnClickEvent::GAME_CREDIT:
			Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushMainGameState(new CreditMenuState());
			break;
		case eOnClickEvent::GAME_QUIT:
			myStateStatus = eStateStatus::ePopMainState;
			break;
		default:
			DL_ASSERT("Unknown event type.");
			break;
		}
	}
}