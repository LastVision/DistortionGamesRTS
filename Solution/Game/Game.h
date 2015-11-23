#pragma once
#include <Matrix.h>
#include <GrowingArray.h>

namespace CU
{
	class InputWrapper;
}

class InGameState;
class MenuState;
class BulletManager;

class Game
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

private:
	void operator=(Game& aApp) = delete;

	CU::InputWrapper* myInputWrapper;
	HWND* myWindowHandler;

	MenuState* myCurrentMenu;
	InGameState* myGame;

	bool myLockMouse;
	bool myShowSystemInfo;
	bool myIsComplete;

	CU::Vector2<int> myWindowSize;

};