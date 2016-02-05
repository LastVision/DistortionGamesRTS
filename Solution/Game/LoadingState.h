#pragma once
#include "GameState.h"

namespace Prism
{
	class SpriteProxy;
}

class LoadingState : public GameState
{
public:
	LoadingState();
	~LoadingState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;

private:
	Prism::SpriteProxy* myBackground;
	Prism::SpriteProxy* myRotatingThing;
	Prism::SpriteProxy* myRotatingThing2;
	//Prism::Text* myText;
	float myRotatingThingScale;

	float myFinishedTextAlpha;
	bool myFinishedTextFadeIn;
	bool myIsDone;

};

