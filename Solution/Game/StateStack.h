#pragma once
#include <GrowingArray.h>

namespace CU
{
	class InputWrapper;
}

class GameState;
class StateStackProxy;

class StateStack
{
public:
	StateStack();
	~StateStack();

	void PushSubGameState(GameState* aSubGameState);
	void PushMainGameState(GameState* aMainGameState);
	bool UpdateCurrentState(const float& aDeltaTime);
	void RenderCurrentState();
	void ResumeState();
	void OnResizeCurrentState(int aWidth, int aHeight);
	void OnResize(int aWidth, int aHeight);
	void Clear();

	void SetInputWrapper(CU::InputWrapper* anInputWrapper);

private:
	CU::GrowingArray<CU::GrowingArray<GameState*, int>, int> myGameStates;
	void PopSubGameState();
	void PopMainGameState();
	void RenderStateAtIndex(int aIndex);

	int myMainIndex;
	int mySubIndex;

	CU::InputWrapper* myInputWrapper;
	StateStackProxy* myStateStackProxy;
};

inline void StateStack::SetInputWrapper(CU::InputWrapper* anInputWrapper)
{
	myInputWrapper = anInputWrapper;
}