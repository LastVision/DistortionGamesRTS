#include "stdafx.h"
#include <InputWrapper.h>
#include "LoadingState.h"
#include <Sprite.h>
#include <SpriteProxy.h>
#include <Text.h>


LoadingState::LoadingState()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	CU::Vector2<float> backgroundSize(2048.f, 1024.f);
	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loadingscreen_background.dds",
		windowSize, windowSize * 0.5f);

	Prism::ModelLoader::GetInstance()->Pause();
	myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
	myText->SetText("Press Enter to start!");
	myText->SetPosition(windowSize * 0.5f);
	Prism::ModelLoader::GetInstance()->UnPause();
}


LoadingState::~LoadingState()
{
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myText);
}

void LoadingState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;

	OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);
}

void LoadingState::EndState()
{
	
}

const eStateStatus LoadingState::Update(const float& aDeltaTime)
{
	if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
	{
		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RETURN))
		{

			return eStateStatus::ePopSubState;
		}
		return eStateStatus::eKeepState;
	}

	return eStateStatus::eKeepState;
}

void LoadingState::Render()
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x),
		float(Prism::Engine::GetInstance()->GetWindowSize().y));
	myBackground->Render(windowSize * 0.5f);

	myText->Render();
}

void LoadingState::ResumeState()
{

}

void LoadingState::OnResize(int aWidth, int aHeight)
{
	CU::Vector2<float> windowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	if (myBackground != nullptr)
	{
		myBackground->SetSize(windowSize, windowSize / 2.f);
	}
}

