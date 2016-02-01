#include "stdafx.h"
#include "InGameMenuState.h"
#include "GUIManager.h"
#include "InputWrapper.h"
#include "PostMaster.h"
#include "OnClickMessage.h"
#include "InGameState.h"


InGameMenuState::InGameMenuState()
{
}


InGameMenuState::~InGameMenuState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void InGameMenuState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/Ingame/GUI_ingame_menu.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
}

void InGameMenuState::EndState()
{

}

void InGameMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

const eStateStatus InGameMenuState::Update(const float& aDeltaTime)
{
	aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopSubState;
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void InGameMenuState::Render()
{
	myGUIManager->Render();
}

void InGameMenuState::ResumeState()
{

}

void InGameMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::RESUME_GAME:
			myIsActiveState = false;
			myStateStatus = eStateStatus::ePopSubState;
			break;
		case eOnClickEvent::GAME_QUIT:
			myIsActiveState = false;
			myStateStatus = eStateStatus::ePopMainState;
			break;

		}
	}
}