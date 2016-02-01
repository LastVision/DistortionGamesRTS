#include "stdafx.h"
#include "TextEventManager.h"
#include <Text.h>

TextEventManager::TextEventManager()
	: myNotificationPosition(500.f, 900.f)
	, myTextLifeTime(5.f)
	, myTextStartFadingTime(1.f)
	, myNotifications(10)
	, myInWorldTexts(10)
	, myInWorldTextIndex(0)
	, myNotificationIndex(0)
{
	for (int i = 0; i < 10; i++)
	{
		InWorldText* inworldText = new InWorldText;
		inworldText->myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
		myInWorldTexts.Add(inworldText);

		NotificationText* notification = new NotificationText;
		notification->myText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE));
		myNotifications.Add(notification);
	}
}

TextEventManager::~TextEventManager()
{
	for (int i = 0; i < myInWorldTexts.Size(); i++)
	{
		SAFE_DELETE(myInWorldTexts[i]->myText);
		SAFE_DELETE(myInWorldTexts[i]);
	}

	for (int i = 0; i < myNotifications.Size(); i++)
	{
		SAFE_DELETE(myNotifications[i]->myText);
		SAFE_DELETE(myNotifications[i]);
	}
}

void TextEventManager::Update(float aDeltaTime)
{
	for (int i = 0; i < myInWorldTexts.Size(); ++i)
	{
		if (myInWorldTexts[i]->myIsActive == true)
		{
			myInWorldTexts[i]->myLifeTime -= aDeltaTime;

			if (myInWorldTexts[i]->myLifeTime <= myTextStartFadingTime)
			{
				myInWorldTexts[i]->myColor.w -= aDeltaTime;
			}
			else if (myInWorldTexts[i]->myLifeTime <= 0.f)
			{
				myInWorldTexts[i]->myIsActive = false;
			}
		}
	}

	for (int i = 0; i < myNotifications.Size(); ++i)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			myNotifications[i]->myLifeTime -= aDeltaTime;

			if (myNotifications[i]->myLifeTime <= myTextStartFadingTime)
			{
				myNotifications[i]->myColor.w -= (1.f - myTextLifeTime - myTextStartFadingTime) * aDeltaTime;
			}
			else  if (myNotifications[i]->myLifeTime <= 0.f)
			{
				myNotifications[i]->myIsActive = false;
			}
		}
	}
}

void TextEventManager::Render()
{
	for (int i = 0; i < myInWorldTexts.Size(); i++)
	{
		if (myInWorldTexts[i]->myIsActive == true)
		{
			myInWorldTexts[i]->myText->Render();
		}
	}

	CU::Vector2<float> position = myNotificationPosition;

	for (int i = 0; i < myNotifications.Size(); i++)
	{
		if (myNotifications[i]->myIsActive == true)
		{
			myNotifications[i]->myText->SetPosition(position);
			myNotifications[i]->myText->SetColor(myNotifications[i]->myColor);
			myNotifications[i]->myText->Render();
			position.y -= 20.f;
		}
	}
}

void TextEventManager::AddNotification(std::string aText, CU::Vector4<float> aColor)
{
	myNotifications[myNotificationIndex]->myText->SetText(aText);
	myNotifications[myNotificationIndex]->myColor = aColor;
	myNotifications[myNotificationIndex]->myLifeTime = myTextLifeTime;
	myNotifications[myNotificationIndex]->myAlphaFade = 1.f;
	myNotifications[myNotificationIndex]->myIsActive = true;

	myNotificationIndex++;

	if (myNotificationIndex >= 10)
	{
		myNotificationIndex = 0;
	}
}

void TextEventManager::AddInWorldText(std::string aText, const CU::Vector2<float>& aPosition, CU::Vector4<float> aColor)
{
	myInWorldTexts[myInWorldTextIndex]->myText->SetText(aText);
	myInWorldTexts[myInWorldTextIndex]->myColor = aColor;
	myInWorldTexts[myInWorldTextIndex]->myLifeTime = myTextLifeTime;
	myInWorldTexts[myInWorldTextIndex]->myAlphaFade = 1.f;
	myInWorldTexts[myInWorldTextIndex]->myIsActive = true;

	myInWorldTextIndex++;

	if (myInWorldTextIndex >= 10)
	{
		myInWorldTextIndex = 0;
	}
}
