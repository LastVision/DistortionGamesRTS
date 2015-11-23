#pragma once
#include "GameState.h"
#include "LevelScore.h"
#include "ScoreItem.h"

namespace Prism
{
	class Sprite;
	class Camera;
}
struct LevelScore;
class GameStateMessage;

class MessageState : public GameState
{
public:
	MessageState(const std::string& aTexturePath, const CU::Vector2<float>& aSize, CU::InputWrapper* anInputWrapper
		, const LevelScore& aLevelScore);
	~MessageState();

	void InitState(StateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

	void SetText(const std::string& aText);
	void SetEvent(GameStateMessage* anEvent);

private:
	void RenderBadgesAndStars(const CU::Vector2<float>& aRenderPos);

	CU::GrowingArray<ScoreItem*> myStars;
	CU::GrowingArray<ScoreItem*> myBadges;


	Prism::Camera* myCamera;
	Prism::Sprite* myBackground;

	Prism::Sprite* myBlackOverlay;

	CU::Vector2<float> mySpriteSize;

	std::string myTextMessage;
	CU::Vector2<float> myMessagePosition;
	GameStateMessage* myEvent;

	LevelScore myLevelScore;
	SaveScore mySaveScore;

	bool myShowBadge;
};

inline void MessageState::SetText(const std::string& aText)
{
	myTextMessage = aText;
}

inline void MessageState::SetEvent(GameStateMessage* anEvent)
{
	myEvent = anEvent;
	myShowBadge = myEvent == nullptr || myEvent->GetGameState() != eGameState::RELOAD_LEVEL;
}
