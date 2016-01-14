#pragma once
#include "GameState.h"
#include <Subscriber.h>

namespace CU
{
	class InputWrapper;
}

namespace Prism
{
	class Camera;
	class Sprite;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class MainMenuState : public GameState, public Subscriber
{
public:
	MainMenuState();
	~MainMenuState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:
	GUI::GUIManager* myGUIManager;

	bool myHasRunOnce;
};

