#pragma once
#include "GameState.h"

class ClickableState : public GameState
{
public:
	ClickableState(float aTimer);
	~ClickableState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;
private:
	float myTimer;
	float myCurrentTime;
};

