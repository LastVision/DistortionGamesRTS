#include "stdafx.h"
#include "SpriteAnimator.h"
#include "SpriteProxy.h"
#include <XMLReader.h>

namespace Prism
{
	SpriteAnimator::SpriteAnimator()
		: myCurrentAnimation(nullptr)
		, myTimeSinceLastFrame(0.f)
		, myCurrentFrame(0)
	{
	}

	SpriteAnimator::SpriteAnimator(std::string aXMLPath)
	{
		XMLReader reader;
		reader.OpenDocument(aXMLPath);

		std::string spritePath = ""; 
		std::string name = "";
		CU::Vector2<float> spriteSize(0.f, 0.f);
		float fps = 0.f; 
		bool isLooping = true;
		int numberOfFrames = -1; 
		int framesPerRow = -1; 
		
		tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");

		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "spritesheet"), "path", spritePath);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "spritesheet"), "sizex", spriteSize.x);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "spritesheet"), "sizey", spriteSize.y);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "name"), "value", name);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "fps"), "value", fps);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "islooping"), "value", isLooping);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "numberofframes"), "value", numberOfFrames);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "framesperrow"), "value", framesPerRow);
		
		if (reader.FindFirstChild(rootElement, "framespercolumn") != nullptr)
		{
			int framesPerColumn = -1;
			reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "framespercolumn"), "value", framesPerColumn);
			AddAnimation(fps, numberOfFrames, framesPerRow, framesPerColumn
				, ModelLoader::GetInstance()->LoadSprite(spritePath, spriteSize), name, isLooping);
		}
		else if (reader.FindFirstChild(rootElement, "framesize") != nullptr)
		{
			CU::Vector2<float> spriteHotspot(0.f, 0.f);
			CU::Vector2<float> frameSize;
			reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "framesize"), "x", frameSize.x);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "framesize"), "y", frameSize.y);
			reader.ReadAttribute(reader.FindFirstChild(rootElement, "framesize"), "hotspotx", spriteHotspot.x);
			reader.ReadAttribute(reader.FindFirstChild(rootElement, "framesize"), "hotspoty", spriteHotspot.y);
			AddAnimation(fps, numberOfFrames, framesPerRow, frameSize
				, ModelLoader::GetInstance()->LoadSprite(spritePath, spriteSize, spriteHotspot), name, isLooping);
		}
		else
		{
			std::string message = "[SpriteAnimation] Has to have either 'framesize' or 'framespercolumn' in " + aXMLPath;
			DL_ASSERT(message);
		}

		reader.CloseDocument();
	}

	SpriteAnimator::~SpriteAnimator()
	{
		for (auto &sprite : myAnimations) 
		{
			SAFE_DELETE(sprite.second);
		}
	}

	void SpriteAnimator::Update(float aDelta)
	{
		if (myCurrentAnimation == nullptr)
		{
			return;
		}
		myTimeSinceLastFrame += aDelta;

		if (myTimeSinceLastFrame >= myCurrentAnimation->GetTimePerFrame())
		{
			myTimeSinceLastFrame -= myCurrentAnimation->GetTimePerFrame();

			myCurrentFrame++;

			if (myCurrentFrame >= myCurrentAnimation->GetNumberOfFrames())
			{
				myCurrentFrame = 0;

				if (myCurrentAnimation->IsLooping() == false)
				{
					myCurrentAnimation = nullptr;
				}
			}
		}
	}

	void SpriteAnimator::Render(CU::Vector2<float> aPosition)
	{
		if (myCurrentAnimation != nullptr)
		{
			GetSpriteFrame()->Render(aPosition);
		}
	}

	void SpriteAnimator::StartAnimation(std::string aName)
	{
		myCurrentAnimation = myAnimations[aName];
	}

	void SpriteAnimator::StopAnimation(std::string aName)
	{
		if (myCurrentAnimation != nullptr && aName == myCurrentAnimation->GetName())
		{
			myCurrentAnimation = nullptr;
		}
	}

	void SpriteAnimator::StopAllAnimations()
	{
		myCurrentAnimation = nullptr;
	}

	void SpriteAnimator::ResetAnimation()
	{
		myCurrentFrame = 0;
	}

	void SpriteAnimator::AddAnimation(float aFPS, int aNumberOfFrames, int aFramesPerRow, int aFramesPerColumn
		, SpriteProxy* aSpriteSheet, std::string aName, bool aIsLooping)
	{
		myAnimations[aName] = new SpriteAnimation(aFPS, aNumberOfFrames, aFramesPerRow, aFramesPerColumn, aSpriteSheet, aName, aIsLooping);
	}

	void SpriteAnimator::AddAnimation(float aFPS, int aNumberOfFrames, int aFramesPerRow, CU::Vector2<float> aFrameSize
		, SpriteProxy* aSpriteSheet, std::string aName, bool aIsLooping)
	{
		myAnimations[aName] = new SpriteAnimation(aFPS, aNumberOfFrames, aFramesPerRow, aFrameSize, aSpriteSheet, aName, aIsLooping);
	}

	void SpriteAnimator::AddAnimation(SpriteAnimation* aAnimation)
	{
		myAnimations[aAnimation->GetName()] = aAnimation;
	}

	SpriteProxy* SpriteAnimator::GetSpriteFrame()
	{
		if (myCurrentAnimation == nullptr)
		{
			return emptySprite;
		}

		int x = myCurrentFrame;
		int y = 0;

		for (; x >= myCurrentAnimation->GetFramesPerRow(); x -= myCurrentAnimation->GetFramesPerRow())
		{
			y++;
		}

		CU::Vector2<float> animationPosition(myCurrentAnimation->GetFrameSize().x * float(x), myCurrentAnimation->GetFrameSize().y * float(y));

		CU::Vector2<float> topLeft = animationPosition;
		CU::Vector2<float> rightBottom = (animationPosition + myCurrentAnimation->GetFrameSize());

		topLeft /= myCurrentAnimation->GetSpriteSheetSize();
		rightBottom /= myCurrentAnimation->GetSpriteSheetSize();

		myCurrentAnimation->GetSpriteSheet()->SetUVZeroToOne(topLeft, rightBottom);
		myCurrentAnimation->GetSpriteSheet()->SetSize(myCurrentAnimation->GetFrameSize(), myCurrentAnimation->GetSpriteSheet()->GetHotspot());
		return myCurrentAnimation->GetSpriteSheet();
	}

	const std::string& SpriteAnimator::GetActiveAnimationName() const
	{
		if (myCurrentAnimation != nullptr)
		{
			return myCurrentAnimation->GetName();
		}

		DL_ASSERT("[SpriteAnimator] Failed to get animation name.");
		return myCurrentAnimation->GetName();
	}
	bool SpriteAnimator::IsPlayingAnimation() const
	{
		return myCurrentAnimation != nullptr;
	}

	int SpriteAnimator::GetCurrentFrame()
	{
		return myCurrentFrame;
	}
}