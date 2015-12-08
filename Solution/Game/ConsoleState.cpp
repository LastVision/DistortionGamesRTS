#include "stdafx.h"
#include "ConsoleState.h"
#include "Console.h"
#include <InputWrapper.h>
#include <Sprite.h>

ConsoleState::ConsoleState(bool& aShouldReOpenConsole)
	: myShouldReOpenConsole(aShouldReOpenConsole)
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

	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	windowSize *= 0.75f;

	myBackground = new Prism::Sprite("Data/Resource/Texture/Console/T_console_window.dds", windowSize);
}

void ConsoleState::EndState()
{
	SAFE_DELETE(myBackground);
}

const eStateStatus ConsoleState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_GRAVE) == true)
	{
		myStateStatus = ePopSubState;
		return eStateStatus::eKeepState;
	}

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_ESCAPE) == true)
	{
		myStateStatus = ePopMainState;
		return eStateStatus::eKeepState;
	}

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_RETURN) == true)
	{
		myShouldReOpenConsole = true;
		myStateStatus = ePopSubState;
	}

	

	//	Console::GetInstance()->Update();

	return myStateStatus;
}

void ConsoleState::Render()
{
	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	windowSize *= 0.25f;
	myBackground->Render(windowSize);


	Prism::Engine::GetInstance()->PrintText(Console::GetInstance()->GetInput(), windowSize * 1.1f, Prism::eTextType::RELEASE_TEXT, 0.9f);

}

void ConsoleState::ResumeState()
{

}

void ConsoleState::OnResize(int aWidth, int aHeight)
{

}