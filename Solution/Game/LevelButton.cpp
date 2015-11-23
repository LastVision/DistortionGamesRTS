#include "stdafx.h"

#include "LevelButton.h"
#include "ScoreIO.h"
#include <Sprite.h>

LevelButton::LevelButton()
{
}

LevelButton::LevelButton(XMLReader& aReader, tinyxml2::XMLElement* aButtonElement, int aLevelID)
	: Button(aReader, aButtonElement, aLevelID)
	, myLevelID(aLevelID)
{
	mySize = CU::Vector2<float>(64.f, 64.f);
	myOptionalBadgeGrey = new Prism::Sprite("Data/Resource/Texture/Menu/BadgeGrey.dds", mySize, mySize * 0.5f);
	myOptionalBadge = new Prism::Sprite("Data/Resource/Texture/Menu/Badge.dds", mySize, mySize * 0.5f);
	myStarGrey = new Prism::Sprite("Data/Resource/Texture/Menu/StarGrey.dds", mySize, mySize * 0.5f);
	myBronzeStar = new Prism::Sprite("Data/Resource/Texture/Menu/StarBronze.dds", mySize, mySize * 0.5f);;
	mySilverStar = new Prism::Sprite("Data/Resource/Texture/Menu/StarSilver.dds", mySize, mySize * 0.5f);;
	myGoldStar = new Prism::Sprite("Data/Resource/Texture/Menu/StarGold.dds", mySize, mySize * 0.5f);;

	myScore = ScoreIO::Load(aLevelID);
}

LevelButton::~LevelButton()
{
	delete myOptionalBadge;
	delete myOptionalBadgeGrey;
	delete myStarGrey;
	delete myBronzeStar;
	delete mySilverStar;
	delete myGoldStar;
	myOptionalBadge = nullptr;
	myOptionalBadgeGrey = nullptr;
	myStarGrey = nullptr;
	myBronzeStar = nullptr;
	mySilverStar = nullptr;
	myGoldStar = nullptr;
}

void LevelButton::Render()
{
	Button::Render();
	for (int i = 0; i < 3; ++i)
	{
		if (i < myScore.myStars)
		{
			if (myScore.myDifficulty == 0)
			{
				myBronzeStar->Render({ myPosition.x + (mySize.x * (i - 1)), myPosition.y - mySize.y * 1.5f });
			}
			else if (myScore.myDifficulty == 1)
			{
				mySilverStar->Render({ myPosition.x + (mySize.x * (i - 1)), myPosition.y - mySize.y * 1.5f });
			}
			else if (myScore.myDifficulty == 2)
			{
				myGoldStar->Render({ myPosition.x + (mySize.x * (i - 1)), myPosition.y - mySize.y * 1.5f });
			}
		}
		else
		{
			myStarGrey->Render({ myPosition.x + (mySize.x * (i - 1)), myPosition.y - mySize.y * 1.5f });
		}
	}

	for (int i = 0; i < myScore.myTotalOptional; ++i)
	{
		if (myScore.myCompletedOptional <= i)
		{
			myOptionalBadgeGrey->Render({ myPosition.x + (mySize.x * (i - 1)), myPosition.y - mySize.y * 2.5f });
		}
		else
		{
			myOptionalBadge->Render({ myPosition.x + (mySize.x * (i - 1)), myPosition.y - mySize.y * 2.5f });
		}
	}
}

void LevelButton::Reload()
{
	myScore = ScoreIO::Load(myLevelID);
}