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

class DifficultySelectState : public GameState, public Subscriber
{
public:
	DifficultySelectState(int aLevelindex);
	~DifficultySelectState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:
	GUI::GUIManager* myGUIManager;
	GUI::GUIManager* myFadingGUIManager;
	int myLevelIndex;
	eDifficulty myDifficulty;
	float myFadeTimer;
	bool myIsFading;
	bool myHasRenderedFadingGUI;
};

