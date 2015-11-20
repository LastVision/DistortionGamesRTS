#include "stdafx.h"
#include <AudioInterface.h>
#include "Menu.h"
#include "Button.h"
#include <Sprite.h>
#include <Camera.h>
#include <InputWrapper.h>
#include "LevelButton.h"
#include "GameStateMessage.h"
#include <XMLReader.h>

Menu::Menu(const std::string& aXMLPath)
	: myButtons(8)
	, myMainMenu(false)
	, myRenderCenter(false)
	, myLevelID(-1)
	, myCredits(nullptr)
{
	XMLReader reader;
	reader.OpenDocument(aXMLPath);

	std::string background;
	std::string crosshair;
	CU::Vector2<float> crosshairSize;

	if (aXMLPath == "Data/Menu/MN_ingame_menu.xml")
	{
		myIsOptionsMenu = true;
	}
	else 
	{
		myIsOptionsMenu = false;
	}

	if (aXMLPath == "Data/Menu/MN_credits.xml")
	{
		myCredits = new Prism::Sprite("Data/Resource/Texture/Menu/Credits/T_credits.dds", { 1024.f, 1024.f }, { 512.f, 512.f });
	}

	tinyxml2::XMLElement* menuElement = reader.FindFirstChild("menu");

	reader.ReadAttribute(menuElement, "mainMenu", myMainMenu);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "path", background);
	reader.ReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "sizeX", myBackgroundSize.x);
	reader.ReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "sizeY", myBackgroundSize.y);
	reader.ReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "renderCenter", myRenderCenter);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "crosshair"), "path", crosshair);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "crosshair"), "sizeX", crosshairSize.x);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "crosshair"), "sizeY", crosshairSize.y);

	myCrosshair = new Prism::Sprite(crosshair, crosshairSize, crosshairSize / 2.f);
	myScreenSize = { float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y) };
	if (myBackgroundSize.x != 0 && myBackgroundSize.y != 0)
	{
		myBackground = new Prism::Sprite(background, myBackgroundSize, myBackgroundSize / 2.f);
	}
	else
	{
		myFullscreenBackgroundSize = { myScreenSize.y * 2.f, myScreenSize.y };
		myBackground = new Prism::Sprite(background, myFullscreenBackgroundSize, myFullscreenBackgroundSize / 2.f);
	}

	tinyxml2::XMLElement* buttonElement = reader.FindFirstChild(menuElement, "button");
	for (; buttonElement != nullptr; buttonElement = reader.FindNextElement(buttonElement))
	{
		Button* newButton = nullptr;
		int id = -1;
		reader.ReadAttribute(buttonElement, "ID", id);
		if (id >= 0)
		{
			 newButton = new LevelButton(reader, buttonElement, id);
		}
		else
		{
			newButton = new Button(reader, buttonElement, -1);
		}
		myButtons.Add(newButton);
	}
	reader.CloseDocument();
}

Menu::Menu(const std::string& aXMLPath, int aLevelID)
	: myButtons(8)
	, myMainMenu(false)
	, myRenderCenter(false)
	, myLevelID(aLevelID)
	, myIsOptionsMenu(false)
{
	XMLReader reader;
	reader.OpenDocument(aXMLPath);

	std::string background;
	std::string crosshair;
	CU::Vector2<float> crosshairSize;

	tinyxml2::XMLElement* menuElement = reader.FindFirstChild("menu");

	reader.ReadAttribute(menuElement, "mainMenu", myMainMenu);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "path", background);
	reader.ReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "sizeX", myBackgroundSize.x);
	reader.ReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "sizeY", myBackgroundSize.y);
	reader.ReadAttribute(reader.ForceFindFirstChild(menuElement, "background"), "renderCenter", myRenderCenter);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "crosshair"), "path", crosshair);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "crosshair"), "sizeX", crosshairSize.x);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(menuElement, "crosshair"), "sizeY", crosshairSize.y);

	myCrosshair = new Prism::Sprite(crosshair, crosshairSize, crosshairSize / 2.f);
	myScreenSize = { float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y) };
	if (myBackgroundSize.x != 0 && myBackgroundSize.y != 0)
	{
		myBackground = new Prism::Sprite(background, myBackgroundSize, myBackgroundSize / 2.f);
	}
	else
	{
		myBackground = new Prism::Sprite(background, myScreenSize, myScreenSize / 2.f);
	}

	tinyxml2::XMLElement* buttonElement = reader.FindFirstChild(menuElement, "button");
	for (; buttonElement != nullptr; buttonElement = reader.FindNextElement(buttonElement))
	{
		Button* newButton = nullptr;
		int id = -1;
		reader.ReadAttribute(buttonElement, "ID", id);
		if (id >= 0)
		{
			newButton = new LevelButton(reader, buttonElement, myLevelID);
		}
		else
		{
			newButton = new Button(reader, buttonElement, myLevelID);
		}
		myButtons.Add(newButton);
	}
	reader.CloseDocument();
}

Menu::~Menu()
{
	myButtons.DeleteAll();
	delete myCrosshair;
	delete myBackground;
	delete myCredits;
	myCrosshair = nullptr;
	myBackground = nullptr;
	myCredits = nullptr;
}

void Menu::Render(CU::InputWrapper* anInputWrapper, bool aRenderButtons, bool aDontRenderSecondButton)
{
	
	myBackground->Render({ (myScreenSize.x / 2.f), -(myScreenSize.y / 2.f) });
	

	if (aRenderButtons == true)
	{
		for (int i = 0; i < myButtons.Size(); i++)
		{
			if (aDontRenderSecondButton == true && i == 1)
			{
				continue;
			}

			myButtons[i]->Render();
		}
	}

	if (myIsOptionsMenu == true)
	{
		Prism::Engine::GetInstance()->PrintText("SFX: " + std::to_string(Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume())
			, { myScreenSize.x / 2.f - 120.f, -myScreenSize.y / 2.f + 65.f }, Prism::eTextType::RELEASE_TEXT);

		Prism::Engine::GetInstance()->PrintText("Music: " + std::to_string(Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume())
			, { myScreenSize.x / 2.f - 120.f, -myScreenSize.y / 2.f - 10.f }, Prism::eTextType::RELEASE_TEXT);

		Prism::Engine::GetInstance()->PrintText("Voice: " + std::to_string(Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume())
			, { myScreenSize.x / 2.f - 120.f, -myScreenSize.y / 2.f - 85.f }, Prism::eTextType::RELEASE_TEXT);
	}

	if (myCredits != nullptr)
	{
		myCredits->Render({ (myScreenSize.x / 2.f), -(myScreenSize.y / 2.f) });
	}

	myCrosshair->Render({ anInputWrapper->GetMousePosition().x, -anInputWrapper->GetMousePosition().y });
}

eStateStatus Menu::Update(CU::InputWrapper* anInputWrapper, bool aUpdateButtons, bool aDontUpdateSecondButton)
{
	eStateStatus returnValue = eStateStatus::eKeepState;

	if (aUpdateButtons == true)
	{
		bool isMouseClicked = anInputWrapper->MouseDown(0);
		CU::Vector2<float> mousePos = anInputWrapper->GetMousePosition();

		for (int i = 0; i < myButtons.Size(); i++)
		{
			if (aDontUpdateSecondButton == true && i == 1)
			{
				continue;
			}

			eStateStatus currentButton = myButtons[i]->Update(mousePos, isMouseClicked, myLevelID);
			if (currentButton == eStateStatus::ePopMainState)
			{
				returnValue = eStateStatus::ePopMainState;
			}
			else if (currentButton == eStateStatus::ePopSubState)
			{
				returnValue = eStateStatus::ePopSubState;
			}
		}
	}
	return returnValue;
}

void Menu::OnResize(int aWidth, int aHeight)
{
	myScreenSize = { float(aWidth), float(aHeight) };

	if (myBackground != nullptr)
	{
		if (myBackgroundSize.x != 0 && myBackgroundSize.y != 0)
		{
			myBackground->SetSize(myBackgroundSize, myBackgroundSize / 2.f);
		}
		else
		{
			myFullscreenBackgroundSize = { myScreenSize.y * 2.f, myScreenSize.y };
			myBackground->SetSize(myFullscreenBackgroundSize, myFullscreenBackgroundSize / 2.f);
		}
	}

	for (int i = 0; i < myButtons.Size(); i++)
	{
		myButtons[i]->OnResize();
	}
}

void Menu::Reload()
{
	for (int i = 0; i < myButtons.Size(); ++i)
	{
		myButtons[i]->Reload();
	}
}