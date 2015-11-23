#pragma once

#include "StateEnums.h"

namespace Prism
{
	class Sprite;
}

namespace tinyxml2
{
	class XMLElement;
}

class GameStateMessage;
class XMLReader;

class Button
{
public:
	Button();
	Button(XMLReader& aReader, tinyxml2::XMLElement* aButtonElement, const int aLevelID);
	~Button();

	virtual void Render();
	eStateStatus Update(const CU::Vector2<float>& aMousePos, const bool& aMouseIsPressed, const int aLevelID);

	void OnResize();

	virtual void Reload(){ };

protected:
	CU::Vector2<float> myPosition;

private:

	CU::Vector2<float> myOriginalPosition;
	CU::Vector2<float> myOriginalSize;

	CU::Vector2<float> mySize;

	Prism::Sprite* myBackground;
	Prism::Sprite* myHoverBackground;
	GameStateMessage* myClickEvent;

	bool myPlayedHover;
	bool myIsHovered;
	bool myBack;
	bool myQuit;
	bool myCalcFromCenter;
	bool myPostSoundEvent;
	std::string myWwiseEvent;
};

