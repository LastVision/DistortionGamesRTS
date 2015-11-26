#pragma once
#include <Matrix.h>
#include <GrowingArray.h>
#include "StateStack.h"
#include <Subscriber.h>

namespace CU
{
	class InputWrapper;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class InGameState;
class MenuState;
class BulletManager;

class Game : public Subscriber
{
public:
	Game();
	~Game();

	bool Init(HWND& aHwnd);
	bool Destroy();
	bool Update();

	void Pause();
	void UnPause();
	void OnResize(int aWidth, int aHeight);

	void ReceiveMessage(const GameStateMessage& aMessage) override;

private:
	void operator=(Game& aApp) = delete;


	GUI::GUIManager* myGUIManager;
	GUI::Cursor* myCursor;

	HWND* myWindowHandler;

	StateStack myStateStack;

	MenuState* myCurrentMenu;
	InGameState* myGame;

	bool myLockMouse;
	bool myShowSystemInfo;
	bool myIsComplete;

	CU::Vector2<int> myWindowSize;

};