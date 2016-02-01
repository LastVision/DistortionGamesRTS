#pragma once

namespace Prism
{
	class Text;
}

struct InWorldText
{
	Prism::Text* myText;
	CU::Vector2<float> myPosition;
	CU::Vector3<float> myCameraPosition;
	CU::Vector4<float> myColor;
	float myLifeTime;
	float myAlphaFade;
	bool myIsActive;
};

struct NotificationText
{
	Prism::Text* myText;
	CU::Vector4<float> myColor;
	float myLifeTime;
	float myAlphaFade;
	bool myIsActive;
};

class TextEventManager
{
public:
	TextEventManager();
	~TextEventManager();

	void Update(float aDeltaTime);
	void Render();

	void AddNotification(std::string aText, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });
	void AddInWorldText(std::string aText, const CU::Vector2<float>& aPosition, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });

private:

	CU::GrowingArray<InWorldText*> myInWorldTexts;
	CU::GrowingArray<NotificationText*> myNotifications;

	CU::Vector2<float> myNotificationPosition;

	int myInWorldTextIndex;
	int myNotificationIndex;

	float myTextLifeTime;
	float myTextStartFadingTime;

};

