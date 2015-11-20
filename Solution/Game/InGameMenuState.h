#pragma once
#include "GameState.h"
#include "Subscriber.h"

namespace Prism
{
	class Sprite;
	class Camera;
}

class Menu;

class InGameMenuState : public GameState, public Subscriber
{
public:
	InGameMenuState(const std::string& aXMLPath, CU::InputWrapper* anInputWrapper, bool anIsIngame = true);
	~InGameMenuState();

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
	bool myIsInGame;
};

