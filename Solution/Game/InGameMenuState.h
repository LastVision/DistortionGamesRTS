#pragma once
#include "Subscriber.h"
#include "GameState.h"

namespace GUI
{
	class GUIManager;
}

class InGameMenuState : public GameState, public Subscriber
{
public:
	InGameMenuState();
	~InGameMenuState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	void OnResize(int aWidth, int aHeight) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void ReceiveMessage(const OnClickMessage& aMessage);

private:
	GUI::GUIManager* myGUIManager;

};