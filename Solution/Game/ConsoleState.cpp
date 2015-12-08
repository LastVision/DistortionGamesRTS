#include "stdafx.h"
#include "ConsoleState.h"
#include "Console.h"
#include "ConsoleHistoryManager.h"
#include <InputWrapper.h>
#include <ScriptSystem.h>
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
	myMarker = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_background.dds", { 2.f, 20.f }, { 0.f, 0.f });
	Console::GetInstance()->GetConsoleHistory()->Load();
	myMarkerBlinker = true;
}

void ConsoleState::EndState()
{
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myMarker);
}

const eStateStatus ConsoleState::Update(const float& aDeltaTime)
{
	Console::GetInstance()->Update();
	
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
		Console::GetInstance()->GetConsoleHistory()->AddHistory(Console::GetInstance()->GetInput());

		LUA::ScriptSystem::GetInstance()->RunLuaFromString(Console::GetInstance()->GetInput());
		Console::GetInstance()->ClearInput();

		Console::GetInstance()->GetConsoleHistory()->Save();
		myStateStatus = ePopSubState;
	}

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_UPARROW) == true)
	{
		Console::GetInstance()->GetConsoleHistory()->GetPrevious();
	}

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_DOWNARROW) == true)
	{
		Console::GetInstance()->GetConsoleHistory()->GetNext();
	}

	myRenderTime += aDeltaTime;
	if (myRenderTime > 0.5f)
	{
		myMarkerBlinker = !myMarkerBlinker;
		myRenderTime = 0.f;
	}

	return myStateStatus;
}

void ConsoleState::Render()
{
	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	windowSize *= 0.25f;
	myBackground->Render(windowSize);


	Prism::Engine::GetInstance()->PrintText(Console::GetInstance()->GetInput(), windowSize * 1.1f, Prism::eTextType::RELEASE_TEXT, 0.9f);

	int length = Console::GetInstance()->GetInput().length();

	if (myMarkerBlinker == true)
	{
		myMarker->Render({ (windowSize.x * 1.1f) + length * 15.f, windowSize.y * 1.1f});
	}

	const CU::GrowingArray<History>& history = Console::GetInstance()->GetConsoleHistory()->GetHistoryArray();
	CU::Vector2<float> position = windowSize * 1.1f;
	position.y += 30.f;

	for (int i = history.Size() - 1; i >= 0; --i)
	{
		position.y += 30.f;
		Prism::Engine::GetInstance()->PrintText(history[i].myHistory, position, Prism::eTextType::RELEASE_TEXT, 0.9f);
	}

}

void ConsoleState::ResumeState()
{

}

void ConsoleState::OnResize(int aWidth, int aHeight)
{

}