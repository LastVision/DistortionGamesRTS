#pragma once
#include "StateEnums.h"
#include "StateStackProxy.h"

namespace CU
{
	class InputWrapper;
}

class StateStackProxy;

class GameState
{
public:

	virtual ~GameState();

	virtual void InitState(StateStackProxy* aStateStackProxy) = 0;
	virtual void EndState() = 0;
	
	virtual const eStateStatus Update(const float& aDeltaTime) = 0;
	virtual void Render() = 0;
	virtual void ResumeState() = 0;
	virtual void OnResize(int aWidth, int aHeight) = 0;

	const bool& IsLetThroughRender() const;

protected:

	CU::InputWrapper* myInputWrapper;
	StateStackProxy* myStateStack;
	eStateStatus myStateStatus;
	bool myIsLetThrough;
	bool myIsActiveState;
};

inline GameState::~GameState()
{

}

inline const bool& GameState::IsLetThroughRender() const
{
	return myIsLetThrough;
}