#include "stdafx.h"

#include "ClickableState.h"
#include <GUIManager.h>
#include <InputWrapper.h>
#include <TextMessage.h>
#include <PostMaster.h>

ClickableState::ClickableState(const float aTime, Prism::Text* aClickableText)
	: myTimer(aTime)
	, myClickableText(aClickableText)
{
	
}

ClickableState::~ClickableState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	myClickableText = nullptr;
	PostMaster::GetInstance()->SendMessage(TextMessage(nullptr));
}

void ClickableState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_tutorial_clickable.xml", nullptr, nullptr, nullptr);
	myIsLetThrough = true;

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	PostMaster::GetInstance()->SendMessage(TextMessage(myClickableText));
}

void ClickableState::EndState()
{

}

const eStateStatus ClickableState::Update(const float& aDeltaTime)
{
	myCurrentTime += aDeltaTime;
	if (myCurrentTime >= myTimer)
	{
		if (CU::InputWrapper::GetInstance()->MouseUp(0) == true)
		{
			myStateStatus = eStateStatus::ePopSubState;
		}
		myCurrentTime = myTimer;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_ESCAPE) == true)
	{
		myStateStatus = eStateStatus::ePopSubState;
	}
	DEBUG_PRINT(myCurrentTime);
	return myStateStatus;
}

void ClickableState::Render()
{
	myGUIManager->Render();
}

void ClickableState::ResumeState()
{

}

void ClickableState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}