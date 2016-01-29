#include "stdafx.h"
#include "SpriteAnimation.h"
#include "SpriteProxy.h"

namespace Prism
{
	SpriteAnimation::SpriteAnimation()
		: mySpriteSheet(nullptr)
	{
	}

	SpriteAnimation::SpriteAnimation(float aTimePerFrame, int aNumberOfFrames, int aFramesPerRow, int aFramesPerColumn
		, SpriteProxy* aSpriteSheet, const std::string &aName, bool aIsLooping)
	{
		myTimePerFrame = aNumberOfFrames / aTimePerFrame;
		myNumberOfFrames = aNumberOfFrames;
		myFramesPerRow = aFramesPerRow;
		mySpriteSheet = aSpriteSheet;
		mySpriteSheetSize = mySpriteSheet->GetSize();
		myIsLooping = aIsLooping;
		myName = aName;

		myFrameSize.x = mySpriteSheet->GetSize().x / myFramesPerRow;
		myFrameSize.y = mySpriteSheet->GetSize().y / aFramesPerColumn;
	}

	SpriteAnimation::SpriteAnimation(float aTimePerFrame, int aNumberOfFrames, int aFramesPerRow, CU::Vector2<float> aFrameSize
		, SpriteProxy* aSpriteSheet, const std::string &aName, bool aIsLooping)
	{
		myTimePerFrame = aNumberOfFrames / aTimePerFrame;
		myNumberOfFrames = aNumberOfFrames;
		myFramesPerRow = aFramesPerRow;
		mySpriteSheet = aSpriteSheet;
		mySpriteSheetSize = mySpriteSheet->GetSize();
		myIsLooping = aIsLooping;
		myName = aName;

		myFrameSize = aFrameSize;
	}

	SpriteAnimation::~SpriteAnimation()
	{
		SAFE_DELETE(mySpriteSheet);
	}

	float SpriteAnimation::GetTimePerFrame() const
	{
		return myTimePerFrame;
	}

	int SpriteAnimation::GetNumberOfFrames() const
	{
		return myNumberOfFrames;
	}

	SpriteProxy* SpriteAnimation::GetSpriteSheet()
	{
		return mySpriteSheet;
	}

	const std::string& SpriteAnimation::GetName() const
	{
		return myName;
	}
}