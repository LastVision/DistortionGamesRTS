#pragma once

#include "StateEnums.h"

namespace Prism
{
	class Sprite;
}

namespace CU
{
	class InputWrapper;
}

class Button;
class GameStateMessage;

class Menu
{
public:
	Menu(const std::string& aXMLPath);
	Menu(const std::string& aXMLPath, int aLevelID);
	~Menu();
	
	void Render(CU::InputWrapper* anInputWrapper, bool aRenderButtons = true, bool aDontRenderSecondButton = false);

	eStateStatus Update(CU::InputWrapper* anInputWrapper, bool aUpdateButtons = true, bool aDontUpdateSecondButton = false);

	void OnResize(int aWidth, int aHeight);

	bool GetMainMenu() const;

	void Reload();

private:
	bool myMainMenu;
	CU::GrowingArray<Button*> myButtons;
	Prism::Sprite* myBackground;
	Prism::Sprite* myCrosshair;
	Prism::Sprite* myCredits;
	CU::Vector2<float> myScreenSize;
	CU::Vector2<float> myBackgroundSize;
	CU::Vector2<float> myFullscreenBackgroundSize;
	bool myRenderCenter;
	bool myIsOptionsMenu;
	int myLevelID;
};

inline bool Menu::GetMainMenu() const
{
	return myMainMenu;
}