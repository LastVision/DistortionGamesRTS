#include "stdafx.h"
#include "ConsoleState.h"
#include "Console.h"
#include <InputWrapper.h>

ConsoleState::ConsoleState()
{
}

ConsoleState::~ConsoleState()
{
}

void ConsoleState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myStateStatus = eStateStatus::eKeepState;
	myIsLetThrough = true;
}

void ConsoleState::EndState()
{

}

const eStateStatus ConsoleState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_GRAVE) == true)
	{
		myStateStatus = ePopSubState;
		return eStateStatus::eKeepState;
	}

	//	Console::GetInstance()->Update();

	return myStateStatus;
}

void ConsoleState::Render()
{
}

void ConsoleState::ResumeState()
{

}

void ConsoleState::OnResize(int aWidth, int aHeight)
{

}