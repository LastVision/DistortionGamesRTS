#include "stdafx.h"
#include <AudioInterface.h>
#include <InputWrapper.h>
#include "LoadingState.h"
#include <Sprite.h>
#include <SpriteProxy.h>
#include <Text.h>


LoadingState::LoadingState()
	: myRotatingThingScale(1.f)
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loadingscreen_background.dds"
		, { windowSize.y * 2.f, windowSize.y }, windowSize * 0.5f);
	myRotatingThing = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loadingscreen_rotating_thing.dds"
		, { 256.f, 256.f }, { 128.f, 128.f });
	myRotatingThing2 = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loadingscreen_rotating_thing_2.dds"
		, { 256.f, 256.f }, { 128.f, 128.f });

	Prism::ModelLoader::GetInstance()->Pause();
	//myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	//myText->SetText("Press Enter to start.");
	//myText->SetPosition(windowSize * 0.5f);
	Prism::ModelLoader::GetInstance()->UnPause();

	myFinishedTextAlpha = 0.f;
	myFinishedTextFadeIn = false;
	myIsDone = false;
}


LoadingState::~LoadingState()
{
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myRotatingThing);
	SAFE_DELETE(myRotatingThing2);
	//SAFE_DELETE(myText);
}

void LoadingState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	aCursor;
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;

	OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Menu", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Loading", 0);
}

void LoadingState::EndState()
{

}

const eStateStatus LoadingState::Update(const float& aDeltaTime)
{
	if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	{
		if (myFinishedTextFadeIn == false)
		{
			myFinishedTextAlpha -= aDeltaTime;
			if (myFinishedTextAlpha <= 0.f)
			{
				myFinishedTextAlpha = 0.f;
				myFinishedTextFadeIn = true;
			}
		}
		else
		{
			myFinishedTextAlpha += aDeltaTime;
			if (myFinishedTextAlpha >= 1.f)
			{
				myFinishedTextAlpha = 1.f;
				myFinishedTextFadeIn = false;
			}
		}
	}

	float speed1 = -3.f;
	float speed2 = 0.0f;

	myRotatingThing->Rotate(speed1 * aDeltaTime * (1.f / (myRotatingThingScale + 0.001f)));
	myRotatingThing2->Rotate(speed2 * aDeltaTime * (1.f / (myRotatingThingScale + 0.001f)));

	if (myRotatingThingScale < 1.f)
	{
		myRotatingThing->Rotate(speed1 * aDeltaTime);
		myRotatingThing2->Rotate(speed2 * aDeltaTime);
	}

	if (Prism::ModelLoader::GetInstance()->IsLoading() == true)
	{
		myRotatingThingScale += aDeltaTime * 2.f;

		if (myRotatingThingScale > 1.f)
		{
			myRotatingThingScale = 1.f;
		}
	}
	else
	{
		myRotatingThingScale -= aDeltaTime * 2.f;

		if (myRotatingThingScale < 0.f)
		{
			myRotatingThingScale = 0.f;
		}

		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE) == true
			|| CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true
			|| CU::InputWrapper::GetInstance()->MouseDown(0) == true
			|| CU::InputWrapper::GetInstance()->MouseDown(1) == true
			|| CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) == true)
		{
			return eStateStatus::ePopSubState;
		}
	}

	return eStateStatus::eKeepState;
	//if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	//{
	//	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RETURN))
	//	{

	//		return eStateStatus::ePopSubState;
	//	}
	//	return eStateStatus::eKeepState;
	//}

	//return eStateStatus::eKeepState;
}

void LoadingState::Render()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));
	myBackground->Render(windowSize * 0.5f);

	//myText->Render();

	if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	{
		Prism::Engine::GetInstance()->PrintText("Press Enter to begin."
			, { windowSize.x * 0.5f - 150.f, windowSize.y  * 0.5f - 350.f }, Prism::eTextType::RELEASE_TEXT
			, 1.f, { 1.f, 1.f, 1.f, myFinishedTextAlpha });
	}

	CU::Vector2<float> position = { windowSize.x * 0.5f + 650.f, windowSize.y *0.5f - 350.f };
	CU::Vector2<float> scale = { myRotatingThingScale * 0.5f, myRotatingThingScale * 0.5f };

	myRotatingThing->Render(position, scale);
	myRotatingThing2->Render(position, scale);
}

void LoadingState::ResumeState()
{

}

void LoadingState::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	if (myBackground != nullptr)
	{
		myBackground->SetSize(windowSize, windowSize / 2.f);
	}
}

