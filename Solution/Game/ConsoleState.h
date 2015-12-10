#pragma once
#include "GameState.h"

namespace Prism
{
	class Sprite;
	class Text;
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
	void operator=(ConsoleState&) = delete;

	Prism::Sprite* myBackground;
	bool& myShouldReOpenConsole;
	Prism::Sprite* myMarker;
	CU::Vector2<float> myMarkerPosition;
	CU::Vector2<float> myLowerLeftCorner;
	Prism::Text* myText;
	float myRenderTime;
	bool myMarkerBlinker;
};

