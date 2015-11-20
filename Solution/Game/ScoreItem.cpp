#include "stdafx.h"
#include <AudioInterface.h>
#include "ScoreItem.h"


ScoreItem::ScoreItem(const LevelScore& aLevelScore, const SaveScore& aSaveScore, bool aStar, int aId)
	: myBackground(nullptr)
	, myItem(nullptr)
	, myScaleDown(false)
	, myWaitDone(false)
	, myShouldPlaySound(false)
{
	float timeDelay = 0.5f;

	mySpriteSize = { 64.f, 64.f };
	if (aStar == true)
	{
		myBackground = new Prism::Sprite("Data/Resource/Texture/Menu/StarGrey.dds", mySpriteSize, mySpriteSize * 0.5f);

		if (aLevelScore.myDifficulty == 0)
		{
			myItem = new Prism::Sprite("Data/Resource/Texture/Menu/StarBronze.dds", mySpriteSize, mySpriteSize * 0.5f);
		}
		else if (aLevelScore.myDifficulty == 1)
		{
			myItem = new Prism::Sprite("Data/Resource/Texture/Menu/StarSilver.dds", mySpriteSize, mySpriteSize * 0.5f);
			
		}
		else if (aLevelScore.myDifficulty == 2)
		{
			myItem = new Prism::Sprite("Data/Resource/Texture/Menu/StarGold.dds", mySpriteSize, mySpriteSize * 0.5f);
		}
		else
		{
			myItem = new Prism::Sprite("Data/Resource/Texture/Menu/StarGold.dds", mySpriteSize, mySpriteSize * 0.5f);
			DL_ASSERT("invalid difficulty");
		}
	}
	else
	{
		myBackground = new Prism::Sprite("Data/Resource/Texture/Menu/BadgeGrey.dds", mySpriteSize, mySpriteSize * 0.5f);
		myItem = new Prism::Sprite("Data/Resource/Texture/Menu/Badge.dds", mySpriteSize, mySpriteSize * 0.5f);
	}

	myWaitTime = (2.f + aId) * timeDelay;
	if (aStar == false)
	{
		myWaitTime += (1.f + float(aSaveScore.myStars)) * timeDelay;
	}
	myTime = 0;
}


ScoreItem::~ScoreItem()
{
	delete myBackground;
	delete myItem;
}


void ScoreItem::Update(float aDeltaTime)
{
	float maxSize = 2.2f;
	float speed = 10.f;
	if (myWaitTime > 0)
	{
		myWaitTime -= aDeltaTime;

		myWaitTime = fmaxf(0, myWaitTime);
	}
	else
	{
		if (myWaitDone == false)
		{
			if (myShouldPlaySound == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ScoreBoom", 0);
			}
			myWaitDone = true;
		}
		myTime += aDeltaTime;
		if (myScaleDown == false)
		{
			myScale = fminf(maxSize, myTime * speed);
		}

		if (myScaleDown == false && myScale >= maxSize)
		{
			myScaleDown = true;
		}
		if (myScaleDown == true)
		{
			myScale -= aDeltaTime * speed;
			myScale = fmaxf(1.f, myScale);
		}
	}
}

void ScoreItem::Render(const CU::Vector2<float>& aPosition, bool aShowItem)
{
	myBackground->Render(aPosition);
	if (aShowItem == true)
	{
		myItem->Render(aPosition, { myScale, myScale });
		myShouldPlaySound = true;
	}
}