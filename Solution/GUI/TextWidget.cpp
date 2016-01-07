#include "stdafx.h"
#include "TextWidget.h"
#include <Engine.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <Text.h>
#include <TextMessage.h>

namespace GUI
{
	TextWidget::TextWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: myBackground(nullptr)
		, myText(new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE)))
	{
		std::string backgroundPath;


		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "background"), "path", backgroundPath);

		myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(backgroundPath, mySize, mySize / 2.f);

		myText->SetText("Hello\nworld");
		PostMaster::GetInstance()->Subscribe(eMessageType::TEXT, this);
	}

	TextWidget::~TextWidget()
	{
		SAFE_DELETE(myBackground);
		SAFE_DELETE(myText);
		PostMaster::GetInstance()->UnSubscribe(eMessageType::TEXT, this);
	}

	void TextWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myText->SetPosition(myPosition + aParentPosition);
		myBackground->Render(myPosition + aParentPosition);
		myText->Render();
	}

	void TextWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myBackground->SetSize(mySize, mySize / 2.f);
	}

	void TextWidget::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myPosition = { aPosition.x + myBackground->GetHotspot().x, aPosition.y - myBackground->GetHotspot().y };
	}

	void TextWidget::ReceiveMessage(const TextMessage& aMessage)
	{
		if (aMessage.myMessageType == eMessageType::TEXT)
		{
			myText->SetText(aMessage.myText);
		}
	}
}