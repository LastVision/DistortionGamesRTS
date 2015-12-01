#include "stdafx.h"
#include <Camera.h>
#include <GUIManager.h>
#include <InputWrapper.h>
#include "MainMenuState.h"
#include <OnClickMessage.h>
#include <PostMaster.h>

MainMenuState::MainMenuState()
{
}

MainMenuState::~MainMenuState()
{
	SAFE_DELETE(myCamera);
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
	CU::GrowingArray<Entity*> temp;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_main_menu.xml", temp);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
}

void MainMenuState::EndState()
{

}

const eStateStatus MainMenuState::Update(const float& aDeltaTime)
{
	aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopMainState;
	}

	myGUIManager->Update();

	return myStateStatus;
}

void MainMenuState::Render()
{
	myGUIManager->Render();
}

void MainMenuState::ResumeState()
{

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
		case eOnClickEvent::QUIT:
			myStateStatus = eStateStatus::ePopMainState;
			break;
		default:
			break;
		}
	}
}