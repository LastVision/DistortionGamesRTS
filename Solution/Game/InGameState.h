#pragma once
#include "GameState.h"
#include <Matrix.h>
#include <GrowingArray.h>
#include <Subscriber.h>

class Level;
class LevelFactory;

namespace CU
{
	class InputWrapper;
}

namespace Prism
{
	class Camera;
	class Sprite;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class InGameState : public GameState, public Subscriber
{
public:

	InGameState();
	~InGameState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const GameStateMessage& aMessage) override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;

	void SetLevel();

	void CompleteLevel();
	void CompleteGame();
	void LoadLevelSettings(); 
	void LoadPlayerSettings();

	bool IsComplete() const;

private:
	void UpdateCamera(float aDeltaTime, const CU::Vector3<float>& aCameraMovement);
	void ShowMessage(const std::string& aBackgroundPath, const CU::Vector2<float>& aSize, std::string aText, GameStateMessage* aMessage = nullptr);

	Level* myLevel;
	LevelFactory* myLevelFactory;
	Prism::Camera* myCamera;
	CU::Matrix44<float> myCameraOrientation;

	GUI::GUIManager* myGUIManager;

	bool myIsComplete;

	bool myRenderGUI;
};

inline bool InGameState::IsComplete() const
{
	return myIsComplete;
}