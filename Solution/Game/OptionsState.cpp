#include "stdafx.h"
#include <AudioInterface.h>
#include "OptionsState.h"
#include "GUIManager.h"
#include "InputWrapper.h"
#include "PostMaster.h"
#include "../Game/GameSettingsSingleton.h"
#include "OnClickMessage.h"
#include "InGameState.h"
#include "HelpState.h"
#include <Text.h>
#include <WidgetContainer.h>

OptionsState::OptionsState()
	: myGUIManager(nullptr)
{
}


OptionsState::~OptionsState()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myMusicText);
	SAFE_DELETE(myShadowText);
	SAFE_DELETE(mySfxText);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void OptionsState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	myCursor = aCursor;
	myIsActiveState = true;
	myIsLetThrough = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_options_menu.xml", nullptr, nullptr, nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();

	myMusicText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	mySfxText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	myShadowText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	OnResize(windowSize.x, windowSize.y);

	if (GameSettingsSingleton::GetInstance()->GetShouldUseShadows() == true)
	{
		myShadowText->SetText("Shadows: ON");
	}
	else
	{
		myShadowText->SetText("Shadows: OFF");
	}
	
	//CU::Vector2<float> floatScreenPos(windowSize.x, windowSize.y);
	//myMusicText->SetPosition({ floatScreenPos.x * 0.5f - 120, floatScreenPos.y * 0.5f });
	//mySfxText->SetPosition({ floatScreenPos.x * 0.5f - 120, floatScreenPos.y * 0.5f + 60.f });

	myMusicVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
	mySfxVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();

	myMusicText->SetText("Music: " + std::to_string(myMusicVolume));
	mySfxText->SetText("SFX: " + std::to_string(mySfxVolume));
}

void OptionsState::EndState()
{
	myIsActiveState = false;
}

void OptionsState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
	CU::Vector2<float> floatScreenPos(aWidth, aHeight);
	GUI::WidgetContainer* widgetCont = reinterpret_cast<GUI::WidgetContainer*>(myGUIManager->GetWidgetContainer()->At(0));
	CU::Vector2<float> firstRow = widgetCont->At(3)->GetPosition();
	CU::Vector2<float> secondRow = widgetCont->At(1)->GetPosition();
	CU::Vector2<float> fourthRow = widgetCont->At(5)->GetPosition();
	myMusicText->SetPosition({ firstRow.x - 320.f, firstRow.y - (widgetCont->At(1)->GetSize().y * 0.25f)});
	mySfxText->SetPosition({ secondRow.x - 320.f, secondRow.y - (widgetCont->At(3)->GetSize().y * 0.25f) });
	myShadowText->SetPosition({ fourthRow.x - 320.f, fourthRow.y });
}

const eStateStatus OptionsState::Update(const float& aDeltaTime)
{
	aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		return eStateStatus::ePopSubState;
	}

	int currentMusicVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
	if (currentMusicVolume != myMusicVolume)
	{
		myMusicVolume = currentMusicVolume;
		myMusicText->SetText("Music: " + std::to_string(myMusicVolume));
	}

	int currentSfxVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
	if (currentSfxVolume != mySfxVolume)
	{
		mySfxVolume = currentSfxVolume;
		mySfxText->SetText("SFX: " + std::to_string(mySfxVolume));
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void OptionsState::Render()
{
	myGUIManager->Render();
	myMusicText->Render();
	mySfxText->Render();
	myShadowText->Render();
}

void OptionsState::ResumeState()
{
	myIsActiveState = true;
}

void OptionsState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::INCREASEVOLUME:
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVolume", 0);
			break;
		case eOnClickEvent::LOWERVOLUME:
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVolume", 0);
			break;
		case eOnClickEvent::INCREASEMUSIC:
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseMusic", 0);
			break;
		case eOnClickEvent::LOWERMUSIC:
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerMusic", 0);
			break;
		case eOnClickEvent::TOGGLE_SHADOWS:
		{
			GameSettingsSingleton::GetInstance()->ToggleShadows();
			if (GameSettingsSingleton::GetInstance()->GetShouldUseShadows() == true)
			{
				myShadowText->SetText("Shadows: ON");
			}
			else
			{
				myShadowText->SetText("Shadows: OFF");
			}
		}
			break;
		case eOnClickEvent::RESUME_GAME:
			myIsActiveState = false;
			myStateStatus = eStateStatus::ePopSubState;
			break;
		default:
			break;
		}
	}
}
