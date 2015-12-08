#include "stdafx.h"
#include "ConsoleState.h"

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
}

void ConsoleState::EndState()
{

}

const eStateStatus ConsoleState::Update(const float& aDeltaTime)
{

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