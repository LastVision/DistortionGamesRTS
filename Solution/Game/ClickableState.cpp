#include "stdafx.h"
#include "ClickableState.h"
#include <InputWrapper.h>

ClickableState::ClickableState(float aTimer)
	: myTimer(aTimer)
{
}

ClickableState::~ClickableState()
{
}

void ClickableState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;

	myIsLetThrough = true;

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
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
	DEBUG_PRINT(myCurrentTime);
	return myStateStatus;
}

void ClickableState::Render()
{
	
}

void ClickableState::ResumeState()
{

}

void ClickableState::OnResize(int, int)
{

}