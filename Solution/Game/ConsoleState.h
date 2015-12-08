#pragma once
#include "GameState.h"

namespace Prism
{
	class Sprite;
}

class ConsoleState : public GameState
{
public:
	ConsoleState(bool& aShouldReOpenConsole);
	~ConsoleState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;


private:
	Prism::Sprite* myBackground;
	bool& myShouldReOpenConsole;
	Prism::Sprite* myMarker;
	float myRenderTime;
	bool myMarkerBlinker;
};

