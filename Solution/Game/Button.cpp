#include "stdafx.h"
#include <AudioInterface.h>
#include "Button.h"
#include <Camera.h>
#include <Engine.h>
#include "GameStateMessage.h"
#include <Sprite.h>
#include "PostMaster.h"
#include <tinyxml2.h>
#include <XMLReader.h>

Button::Button()
{
}

Button::Button(XMLReader& aReader, tinyxml2::XMLElement* aButtonElement, const int aLevelID)
	: myClickEvent(nullptr)
	, myBack(false)
	, myCalcFromCenter(false)
	, myQuit(false)
	, myPostSoundEvent(false)
	, myPlayedHover(false)
{
	std::string picPath;
	std::string picHoveredPath;
	std::string eventType;
	
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "position"), "startFromCenter", myCalcFromCenter);
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "position"), "x", myOriginalPosition.x);
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "position"), "y", myOriginalPosition.y);
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "picture"), "path", picPath);
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "picture"), "sizeX", myOriginalSize.x);
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "picture"), "sizeY", myOriginalSize.y);
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "hoveredPicture"), "path", picHoveredPath);
	aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "onClick"), "event", eventType);

	if (myCalcFromCenter == true)
	{
		myOriginalPosition.x += Prism::Engine::GetInstance()->GetWindowSize().x / 2.f;
		myOriginalPosition.y += -Prism::Engine::GetInstance()->GetWindowSize().y / 2.f;
	}

	if (eventType == "level")
	{
		int levelID;
		int difficultID;
		aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "onClick"), "ID", levelID);
		aReader.ForceReadAttribute(aReader.FindFirstChild(aButtonElement, "onClick"), "difficulty", difficultID);
		myClickEvent = new GameStateMessage(eGameState::LOAD_GAME, aLevelID, difficultID);
	}
	else if (eventType == "menu")
	{
		std::string menuID;
		aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "onClick"), "ID", menuID);
		myClickEvent = new GameStateMessage(eGameState::LOAD_MENU, menuID);
	}
	else if (eventType == "difficultMenu")
	{
		std::string menuID;
		int levelID;
		aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "onClick"), "ID", menuID);
		aReader.ReadAttribute(aReader.FindFirstChild(aButtonElement, "onClick"), "level", levelID);
		myClickEvent = new GameStateMessage(eGameState::LOAD_MENU, menuID, levelID);
	}
	else if (eventType == "optionsMenu")
	{
		myClickEvent = new GameStateMessage(eGameState::LOAD_MENU, -2);
	}
	else if (eventType == "back")
	{
		myBack = true;
	}
	else if (eventType == "quit")
	{
		myQuit = true;
	}
	else if (eventType == "wwise")
	{
		myPostSoundEvent = true;
		aReader.ReadAttribute(aReader.ForceFindFirstChild(aButtonElement, "onClick"), "eventname", myWwiseEvent);
	}
	
	OnResize();

	myBackground = new Prism::Sprite(picPath, mySize, mySize / 2.f);

	myHoverBackground = new Prism::Sprite(picHoveredPath, mySize, mySize / 2.f);

	myIsHovered = false;
}

Button::~Button()
{
	delete myBackground;
	delete myClickEvent;
	myBackground = nullptr;
	myClickEvent = nullptr;
}

void Button::Render()
{
	if (myIsHovered == false)
	{
		myBackground->Render({ myPosition.x, myPosition.y });
	}
	else
	{
		myHoverBackground->Render({ myPosition.x, myPosition.y });
	}
}

eStateStatus Button::Update(const CU::Vector2<float>& aMousePos, const bool& aMouseIsPressed, const int aLevelID)
{
	aLevelID;
	myIsHovered = false;

	if ((aMousePos.x + mySize.x / 2) >= myPosition.x && 
		(aMousePos.x + mySize.x / 2) <= myPosition.x + mySize.x &&
		(-aMousePos.y - mySize.y / 2) <= myPosition.y &&
		(-aMousePos.y - mySize.y / 2) >= myPosition.y - mySize.y)
	{
		if (myPlayedHover == false)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ButtonHovered", 0);
			myPlayedHover = true;
		}
		if (aMouseIsPressed == true)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ButtonClicked", 0);
			if (myBack == true)
			{
				return eStateStatus::ePopSubState;
			}
			else if (myQuit == true)
			{
				return eStateStatus::ePopMainState;
			}
			else if (myClickEvent != nullptr)
			{
				PostMaster::GetInstance()->SendMessage(*myClickEvent);
			}
			else if (myPostSoundEvent == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent(myWwiseEvent.c_str(), 0);
			}
		}
		myIsHovered = true;
	}
	else
	{
		myPlayedHover = false;
	}
	return eStateStatus::eKeepState;
}

void Button::OnResize()
{
	//float windowSizeX = static_cast<float>(Prism::Engine::GetInstance()->GetWindowSize().x);
	//float windowSizeY = static_cast<float>(Prism::Engine::GetInstance()->GetWindowSize().y);
	//float resolutionOffset = windowSizeY / windowSizeX;
	myPosition = myOriginalPosition; // *resolutionOffset;

	// ger lite weird resultat
	mySize = myOriginalSize;// *resolutionOffset;
}