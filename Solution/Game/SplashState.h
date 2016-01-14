#pragma once
#include "GameState.h"

namespace Prism
{
	class Sprite;
	class Camera;
}

class Menu;

class SplashState : public GameState
{
public:
	SplashState(const std::string& aTexturePath, bool aVictoryScreen = false);
	~SplashState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

protected:
	bool myFadeOut;
	bool myDisplay;
	float myLogoAlpha;

	float myFadeInTime;
	float myFadeOutTime;
	float myDisplayTime;
	float myCurrentTime;
	Prism::Sprite* myBackground;
	Prism::Sprite* myLogo;
	Prism::Camera* myCamera;

	bool myVictoryScreen;
};

