#pragma once

namespace Prism
{
	class Sprite;
}

namespace CU
{
	class InputWrapper;
}

struct TimedMessage
{
	std::string myMessage;
	float myMaxTime;
	float myCurrentTime;
};

struct Screen
{
	Screen();
	~Screen();
	Prism::Sprite* myBackground;
	Prism::Sprite* myRotatingThing;
	Prism::Sprite* myRotatingThing2;
	CU::GrowingArray<TimedMessage> myMessages;
	float myRotatingThingScale;
};

class LoadingScreen
{
public:
	LoadingScreen(CU::InputWrapper* anInputWrapper, const volatile bool& aLevelIsLoading, const int& aLevelID);
	~LoadingScreen();

	void Start(int aLevelID);

	void Render();
	void Update(float aDeltaTime);

	void OnResize(int aWidth, int aHeight);

	void Reset();

	bool IsDone() const;

private:

	void ReadXML();

	void operator=(const LoadingScreen&) = delete;

	CU::GrowingArray<Screen*> myScreens;
	CU::Vector2<float> myWindowMiddle;

	CU::InputWrapper* myInputWrapper;

	bool myIsDone;

	int myTextIndex;

	const volatile bool& myLevelIsLoading;
	const int& myLevelID;

	float myFinishedTextAlpha;
	bool myFinishedTextFadeIn;
};

inline bool LoadingScreen::IsDone() const
{
	return myIsDone && !myLevelIsLoading;
}