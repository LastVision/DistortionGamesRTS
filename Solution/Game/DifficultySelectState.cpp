#include "stdafx.h"
#include <Camera.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "DifficultySelectState.h"
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "StateStackProxy.h"
#include <FadeMessage.h>

DifficultySelectState::DifficultySelectState(int aLevelindex)
	: myLevelIndex(aLevelindex)
	, myGUIManager(nullptr)
	, myFadingGUIManager(nullptr)
	, myFadeTimer(0.f)
	, myIsFading(false)
	, myHasRenderedFadingGUI(false)
{
}

DifficultySelectState::~DifficultySelectState()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myFadingGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void DifficultySelectState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_difficulty_select.xml", nullptr, nullptr, nullptr, -1);
	myFadingGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_level_select.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
	myFadeTimer = 1.f / 3.f;
	myIsFading = false;
	myHasRenderedFadingGUI = false;
}

void DifficultySelectState::EndState()
{

}

const eStateStatus DifficultySelectState::Update(const float& aDeltaTime)
{
	aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopMainState;
	}

	

	if (myIsFading == true)
	{
		myFadeTimer -= aDeltaTime;
		if (myFadeTimer < 0.f)
		{
			myStateStack->PushSubGameState(new InGameState(myLevelIndex, myDifficulty));
			myFadeTimer = 0.f;
		}
	}
	else
	{
		myGUIManager->Update(aDeltaTime);
	}

	return myStateStatus;
}

void DifficultySelectState::Render()
{
	
	if (myIsFading == true)
	{
		myFadingGUIManager->Render();
	}
	else
	{
		myGUIManager->Render();

	}
}

void DifficultySelectState::ResumeState()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
	myFadeTimer = 1.f / 3.f;
	myIsFading = false;
	myHasRenderedFadingGUI = false;
}

void DifficultySelectState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
	myFadingGUIManager->OnResize(aWidth, aHeight);
}

void DifficultySelectState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::GAME_START_EASY:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myDifficulty = eDifficulty::EASY;
			myIsFading = true;
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
			//myStateStack->PushSubGameState(new InGameState(myLevelIndex, eDifficulty::EASY));
			break;
		case eOnClickEvent::GAME_START_NORMAL:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myDifficulty = eDifficulty::NORMAL;
			myIsFading = true;
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
			//myStateStack->PushSubGameState(new InGameState(myLevelIndex, eDifficulty::NORMAL));
			break;
		case eOnClickEvent::GAME_START_HARD:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myDifficulty = eDifficulty::HARD;
			myIsFading = true;
			PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
			//myStateStack->PushSubGameState(new InGameState(myLevelIndex, eDifficulty::HARD));
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