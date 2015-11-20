#include "stdafx.h"
//#include <Engine.h>
#include "LoadingScreen.h"
#include <InputWrapper.h>
#include <Sprite.h>
#include <XMLReader.h>

Screen::Screen()
	: myBackground(nullptr)
	, myRotatingThing(nullptr)
	, myMessages(8)
	, myRotatingThingScale(0.f)
{
};
Screen::~Screen()
{
	delete myBackground;
	myBackground = nullptr;
	delete myRotatingThing;
	myRotatingThing = nullptr;
};

LoadingScreen::LoadingScreen(CU::InputWrapper* anInputWrapper, const volatile bool& aLevelIsLoading, const int& aLevelID)
	: myScreens(8)
	, myTextIndex(0)
	, myIsDone(false)
	, myInputWrapper(anInputWrapper)
	, myLevelID(aLevelID)
	, myLevelIsLoading(aLevelIsLoading)
	, myFinishedTextAlpha(1.f)
	, myFinishedTextFadeIn(true)
{
	OnResize(Prism::Engine::GetInstance()->GetWindowSize().x, Prism::Engine::GetInstance()->GetWindowSize().y);
	ReadXML();
}

LoadingScreen::~LoadingScreen()
{
	myScreens.DeleteAll();
}

void LoadingScreen::Render()
{
	myScreens[myLevelID - 1]->myBackground->Render(myWindowMiddle);

	Prism::Engine::GetInstance()->PrintText(myScreens[myLevelID - 1]->myMessages[0].myMessage
		, { myWindowMiddle.x - 425.f, myWindowMiddle.y + 400.f }, Prism::eTextType::RELEASE_TEXT);

	if (myLevelIsLoading == false)
	{
		Prism::Engine::GetInstance()->PrintText("Press [Space] to begin."
			, { myWindowMiddle.x - 300.f, myWindowMiddle.y - 300.f }, Prism::eTextType::RELEASE_TEXT
			, 1.f, { 1.f, 1.f, 1.f, myFinishedTextAlpha});
	}

	myScreens[myLevelID - 1]->myRotatingThing->Render({ myWindowMiddle.x + 500.f, myWindowMiddle.y - 300.f }
	, { myScreens[myLevelID - 1]->myRotatingThingScale, myScreens[myLevelID - 1]->myRotatingThingScale });
	myScreens[myLevelID - 1]->myRotatingThing2->Render({ myWindowMiddle.x + 500.f, myWindowMiddle.y - 300.f }
	, { myScreens[myLevelID - 1]->myRotatingThingScale, myScreens[myLevelID - 1]->myRotatingThingScale });
}

void LoadingScreen::Update(float aDeltaTime)
{
	if (myLevelIsLoading == false)
	{
		if (myFinishedTextFadeIn == false)
		{
			myFinishedTextAlpha -= aDeltaTime;
			if (myFinishedTextAlpha <= 0.f)
			{
				myFinishedTextAlpha = 0.f;
				myFinishedTextFadeIn = true;
			}
		}
		else
		{
			myFinishedTextAlpha += aDeltaTime;
			if (myFinishedTextAlpha >= 1.f)
			{
				myFinishedTextAlpha = 1.f;
				myFinishedTextFadeIn = false;
			}
		}
	}

	float speed1 = -3.f;
	float speed2 = 0.2f;

	myScreens[myLevelID - 1]->myRotatingThing->Rotate(speed1 * aDeltaTime * (1.f / (myScreens[myLevelID - 1]->myRotatingThingScale + 0.001f)));
	myScreens[myLevelID - 1]->myRotatingThing2->Rotate(speed2 * aDeltaTime * (1.f / (myScreens[myLevelID - 1]->myRotatingThingScale + 0.001f)));

	if (myScreens[myLevelID - 1]->myRotatingThingScale < 1.f)
	{
		myScreens[myLevelID - 1]->myRotatingThing->Rotate(speed1 * aDeltaTime);
		myScreens[myLevelID - 1]->myRotatingThing2->Rotate(speed2 * aDeltaTime);
	}

	if (myLevelIsLoading == true)
	{
		myScreens[myLevelID - 1]->myRotatingThingScale += aDeltaTime * 2.f;

		if (myScreens[myLevelID - 1]->myRotatingThingScale > 1.f)
		{
			myScreens[myLevelID - 1]->myRotatingThingScale = 1.f;
		}
	}
	else
	{
		myScreens[myLevelID - 1]->myRotatingThingScale -= aDeltaTime * 2.f;

		if (myScreens[myLevelID - 1]->myRotatingThingScale < 0.f)
		{
			myScreens[myLevelID - 1]->myRotatingThingScale = 0.f;
		}

		if (myInputWrapper->KeyDown(DIK_SPACE) == true || myInputWrapper->KeyDown(DIK_ESCAPE) == true)
		{
			myIsDone = true;
			return;
		}
	}
}

void LoadingScreen::OnResize(int aWidth, int aHeight)
{
	// set size

	myWindowMiddle.x = float(aWidth) / 2.f;
	myWindowMiddle.y = -float(aHeight) / 2.f;
}

void LoadingScreen::Reset()
{
	myIsDone = false;
	myTextIndex = 0;

	for (int i = 0; i < myScreens.Size(); i++)
	{
		for (int j = 0; j < myScreens[i]->myMessages.Size(); j++)
		{
			myScreens[i]->myMessages[j].myCurrentTime = 0.f;
		}
	}
}

void LoadingScreen::ReadXML()
{
	CU::Vector2<float> windowSize = { float(Prism::Engine::GetInstance()->GetWindowSize().x), float(Prism::Engine::GetInstance()->GetWindowSize().y) };
	XMLReader reader;
	reader.OpenDocument("Data/Menu/MN_loading_screen.xml");
	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");

	for (tinyxml2::XMLElement* screenElement = reader.FindFirstChild(rootElement, "screen"); screenElement != nullptr;
		screenElement = reader.FindNextElement(screenElement, "screen"))
	{
		std::string aSpritePath = "";
		int levelID = 0;

		reader.ForceReadAttribute(reader.FindFirstChild(screenElement, "ID"), "value", levelID);
		if (levelID != myScreens.Size())
		{
			DL_ASSERT("[LoadingScreen] Wrong ID-number in MN_loading_screen.xml! The numbers should be counting up, in order, starting from 0.");
		}
		
		std::string aRotatingThingPath = "";
		std::string aRotatingThingPath2 = "";

		reader.ForceReadAttribute(reader.FindFirstChild(screenElement, "background"), "path", aSpritePath);
		reader.ForceReadAttribute(reader.FindFirstChild(screenElement, "rotatingThing"), "path", aRotatingThingPath);
		reader.ForceReadAttribute(reader.FindFirstChild(screenElement, "rotatingThing2"), "path", aRotatingThingPath2);
		
		Screen* newScreen = new Screen();
		newScreen->myBackground = new Prism::Sprite(aSpritePath, { windowSize.y * 2.f, windowSize.y }, windowSize / 2.f);
		newScreen->myRotatingThing = new Prism::Sprite(aRotatingThingPath, { 256.f, 256.f }, { 128.f, 128.f });
		newScreen->myRotatingThing2 = new Prism::Sprite(aRotatingThingPath2, { 256.f, 256.f }, { 128.f, 128.f });



		tinyxml2::XMLElement* textElement = reader.FindFirstChild(screenElement, "text");

		TimedMessage newMessage;
		newMessage.myCurrentTime = 0.f;

		reader.ForceReadAttribute(textElement, "value", newMessage.myMessage);
		reader.ForceReadAttribute(textElement, "time", newMessage.myMaxTime);

		newScreen->myMessages.Add(newMessage);

		myScreens.Add(newScreen);
	}

	reader.CloseDocument();
}