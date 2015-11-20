#pragma once
#include "LevelScore.h"
#include <Sprite.h>

class ScoreItem
{
public:
	ScoreItem(const LevelScore& aLevelScore, const SaveScore& aSaveScore, bool aStar, int aId);
	~ScoreItem();

	void Update(float aDeltaTime);
	void Render(const CU::Vector2<float>& aPosition, bool aShowItem);

private:

	Prism::Sprite* myBackground;
	Prism::Sprite* myItem;
	float myTime;
	float myWaitTime;
	CU::Vector2<float> mySpriteSize;
	float myScale;
	bool myScaleDown;
	bool myWaitDone;
	bool myShouldPlaySound;
};

