#pragma once
#include "GameState.h"

namespace Prism
{
	class Sprite;
	class Camera;
}

class Menu;

class MenuState : public GameState
{
public:
	MenuState(const std::string& aXMLPath, CU::InputWrapper* anInputWrapper, int aLevelID = -1, bool aShowVictoryScreen = false);
	~MenuState();

	void InitState(StateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

protected:
	Prism::Sprite* myBlackOverlay;
	float myFadeInTime;
	float myCurrentTime;
	float myOverlayAlpha;
	Menu* myMenu;
	Prism::Camera* myCamera;
	bool myHasFadeIn;
	bool myShowVictoryScreen;

	bool myShowButtons;

	float myWaitForButtonsTime;
};

