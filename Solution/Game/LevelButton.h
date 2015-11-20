#pragma once
#include "Button.h"
#include "LevelScore.h"
#include <Vector.h>

class LevelButton : public Button
{
public:
	LevelButton();
	LevelButton(XMLReader& aReader, tinyxml2::XMLElement* aButtonElement, int aLevelID);
	~LevelButton();

	void Render() override;

	void Reload() override;

private:
	SaveScore myScore;
	Prism::Sprite* myOptionalBadgeGrey;
	Prism::Sprite* myOptionalBadge;
	Prism::Sprite* myStarGrey;
	Prism::Sprite* myBronzeStar;
	Prism::Sprite* myGoldStar;
	Prism::Sprite* mySilverStar;

	CU::Vector2<float> mySize;
	int myLevelID;
};

