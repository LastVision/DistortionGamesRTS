#include "stdafx.h"
#include "ConsoleState.h"
#include "Console.h"
#include "ConsoleHelp.h"
#include "ConsoleHistoryManager.h"
#include <InputWrapper.h>
#include <ScriptSystem.h>
#include <Sprite.h>
#include <Text.h>

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
	myMarkerBlinker = true;

	myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));

	myLowerLeftCorner = Prism::Engine::GetInstance()->GetWindowSize();
	myLowerLeftCorner *= 0.25f;
}

void ConsoleState::EndState()
{
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myMarker);
	SAFE_DELETE(myText);
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
		const std::string& consoleInput = Console::GetInstance()->GetInput();
		Console::GetInstance()->GetConsoleHistory()->AddHistory(consoleInput);
		std::string errorString;
		std::string temp = CU::ToLower(consoleInput);
		if (temp.find("help") == 0 || temp.find("halp") == 0)
		{
			if (temp == "help" || temp == "halp")
			{
				const CU::GrowingArray<std::string>& allFunctions = Console::GetInstance()->GetConsoleHelp()->GetAllFunction();
				for (int i = 0; i < allFunctions.Size(); ++i)
				{
					Console::GetInstance()->GetConsoleHistory()->AddHistory(allFunctions[i], eHistoryType::HELP);
				}
			}
			else 
			{
				std::string helpFunctionName = CU::GetSubString(temp, " ", true, 2);
				const ConsoleLuaHelp& helpDoc = Console::GetInstance()->GetConsoleHelp()->GetHelpText(helpFunctionName);
				if (helpDoc.myFunctionName != "")
				{
					Console::GetInstance()->GetConsoleHistory()->AddHistory(helpDoc.myFunctionName + "(" + helpDoc.myArguments + ")", eHistoryType::HELP);
					Console::GetInstance()->GetConsoleHistory()->AddHistory(helpDoc.myHelpText, eHistoryType::HELP);
				}
				else 
				{
					Console::GetInstance()->GetConsoleHistory()->AddHistory("There is no such command. Did you mean <insert command>?", eHistoryType::ERROR);
				}
			}
			Console::GetInstance()->ClearInput();
		}
		else if (LUA::ScriptSystem::GetInstance()->ValidateLuaString(consoleInput, errorString))
		{
			LUA::ScriptSystem::GetInstance()->RunLuaFromString(consoleInput);
			Console::GetInstance()->ClearInput();
		}
		else
		{
			Console::GetInstance()->GetConsoleHistory()->AddHistory(errorString, eHistoryType::ERROR);
		}
		

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

	myText->SetText(Console::GetInstance()->GetInput());
	myMarkerPosition.x = (myLowerLeftCorner.x * 1.1f) + myText->GetWidth() + 3;
	myMarkerPosition.y = myLowerLeftCorner.y * 1.1f - 3;

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

	myBackground->Render(myLowerLeftCorner);

	myText->SetPosition(myLowerLeftCorner * 1.1f);
	myText->SetScale({ 0.9f, 0.9f });
	myText->SetColor({ 1.f, 1.f, 1.f, 1.f });
	myText->Render();
	//Prism::Engine::GetInstance()->RenderText(myText);
	//Prism::Engine::GetInstance()->PrintText(myText->myText , myLowerLeftCorner * 1.1f, Prism::eTextType::RELEASE_TEXT, 0.9f);

	//int length = Console::GetInstance()->GetInput().length();

	if (myMarkerBlinker == true)
	{
		myMarker->Render(myMarkerPosition);
		//myMarker->Render({ (myLowerLeftCorner.x * 1.1f) + length * 15.f, myLowerLeftCorner.y * 1.1f });
	}

	const CU::GrowingArray<History*>& history = Console::GetInstance()->GetConsoleHistory()->GetHistoryArray();
	CU::Vector2<float> position = myLowerLeftCorner * 1.1f;
	position.y += 30.f;

	for (int i = history.Size() - 1; i >= 0; --i)
	{
		position.y += 30.f;
		history[i]->myRenderText->SetPosition(position);
		history[i]->myRenderText->Render();
	}

}

void ConsoleState::ResumeState()
{

}

void ConsoleState::OnResize(int aWidth, int aHeight)
{

}