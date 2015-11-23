#include "stdafx.h"

#include "PowerUpGUIIcon.h"
#include "Sprite.h"

PowerUpGUIIcon::PowerUpGUIIcon(const std::string& aActivePath, const std::string& aInactivePath
		, const CU::Vector2<float>& aPosition, const bool& aActive, const float* aDuration)
	: myPosition(aPosition)
	, myActive(aActive)
	, myDuration(aDuration)
	, myLastFrameActive(false)
	, myAlpha(0.0f)
	, myFadeUp(false)
{
	CU::Vector2<float> size = { 64.f, 64.f };
	myActiveIcon = new Prism::Sprite(aActivePath, size, size * 0.5f);
	myInactiveIcon = new Prism::Sprite(aInactivePath, size, size * 0.5f);
	myTextPosition = myPosition + CU::Vector2<float>(size.x * 0.75f, 0.f);
}


PowerUpGUIIcon::~PowerUpGUIIcon()
{
	delete myInactiveIcon;
	delete myActiveIcon;
	myActiveIcon = nullptr;
	myInactiveIcon = nullptr;
}

void PowerUpGUIIcon::Render(const CU::Vector2<int>& aWindowSize, float aDeltaTime)
{
	CU::Vector2<float> screenCenter(float(aWindowSize.x) * 0.5f, -float(aWindowSize.y) * 0.5f);
	if (myActive == true && myLastFrameActive == false)
	{
		myFadeUp = true;
	}
	if (myFadeUp == true)
	{
		myAlpha += 4 * aDeltaTime;
		if (myAlpha > 1.f)
		{
			myAlpha = 1.f;
			myFadeUp = false;
		}
	}

	if (myActive == true)
	{
		myActiveIcon->Render(screenCenter + myPosition, { myAlpha, myAlpha }, { 1.f, 1.f, 1.f, myAlpha });
		if (myDuration != nullptr)
		{
			//rendertext på min pos
			Prism::Engine::GetInstance()->PrintText(*myDuration, screenCenter + myTextPosition, Prism::eTextType::RELEASE_TEXT);
		}
	}
	else
	{
		myInactiveIcon->Render(screenCenter + myPosition);
	}
	myLastFrameActive = myActive;
}
