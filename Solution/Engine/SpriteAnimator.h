#pragma once
#include <map>
#include "SpriteAnimation.h"

namespace Prism
{
	class SpriteProxy;

	class SpriteAnimator
	{
	public:
		SpriteAnimator();
		SpriteAnimator(std::string aXMLPath);
		~SpriteAnimator();

		void Update(float aDelta);
		void Render(CU::Vector2<float> aPosition);

		void StartAnimation(std::string aName);
		void StopAnimation(std::string aName);
		void StopAllAnimations();
		void ResetAnimation();

		void AddAnimation(float aFPS, int aNumberOfFrames, int aFramesPerRow, int aFramesPerColumn
			, SpriteProxy* aSpriteSheet, std::string aName, bool aIsLooping);
		void AddAnimation(float aFPS, int aNumberOfFrames, int aFramesPerRow, CU::Vector2<float> aFrameSize
			, SpriteProxy* aSpriteSheet, std::string aName, bool aIsLooping);
		void AddAnimation(SpriteAnimation* aAnimation);

		SpriteProxy* GetSpriteFrame();
		const std::string& GetActiveAnimationName()const;
		bool IsPlayingAnimation() const;
		int GetCurrentFrame();

	private:
		float myTimeSinceLastFrame;
		int myCurrentFrame;
		std::map<std::string, SpriteAnimation*> myAnimations;
		SpriteAnimation *myCurrentAnimation;
		SpriteProxy* emptySprite;
	};
}