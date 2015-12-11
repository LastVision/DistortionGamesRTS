#pragma once

struct AnimationLoadData
{
	eEntityState myEntityState;
	const char* myAnimationPath;
	bool myLoopFlag;
	bool myResetTimeOnRestart;
};

struct AnimationComponentData
{
	bool myExistsInEntity = false;
	const char* myModelPath;
	const char* myEffectPath;
	CU::StaticArray<AnimationLoadData, static_cast<int>(eEntityState::_COUNT)> myAnimations;
};