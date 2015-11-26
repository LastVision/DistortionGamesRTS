#include "stdafx.h"
#include "ButtonWidget.h"
#include <Sprite.h>
#include <XMLReader.h>

namespace GUI
{
	ButtonWidget::ButtonWidget()
		: myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
	{
	}

	ButtonWidget::ButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
	{
		std::string spritePathNormal = "";
		std::string spritePathHover = "";
		std::string spritePathPressed = "";
		std::string hoverText = "";
		std::string clickEvent = "";

		CU::Vector2<float> size;
		CU::Vector2<float> position;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritenormal"), "path", spritePathNormal);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritehover"), "path", spritePathHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritepressed"), "path", spritePathPressed);

		aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "hover"), "text", hoverText);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "event", clickEvent);

		if (clickEvent == "lose")
		{
		
		}
		else if (clickEvent == "win")
		{
		
		}
		else if (clickEvent == "restart")
		{

		}
		else if (clickEvent == "quit")
		{

		}

		mySize = size;
		myPosition = position;
		myHoverText = hoverText;

		myImageNormal = new Prism::Sprite(spritePathNormal, mySize, mySize / 2.f);
		myImageHover = new Prism::Sprite(spritePathHover, mySize, mySize / 2.f);
		myImagePressed = new Prism::Sprite(spritePathPressed, mySize, mySize / 2.f);
		myImageCurrent = myImageNormal;
	}

	ButtonWidget::~ButtonWidget()
	{
		delete myImageNormal;
		delete myImagePressed;
		delete myImageHover;
		myImageNormal = nullptr;
		myImagePressed = nullptr;
		myImageHover = nullptr;
		myImageCurrent = nullptr;
	}

	void ButtonWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myImageCurrent->Render(myPosition - aParentPosition);
	}

	void ButtonWidget::OnMousePressed(const CU::Vector2<float>&)
	{
		myImageCurrent = myImagePressed;
		Click();
	}

	void ButtonWidget::OnMouseDown(const CU::Vector2<float>&)
	{
		Click();
	}

	void ButtonWidget::OnMouseUp()
	{
		myImageCurrent = myImageNormal;
	}

	void ButtonWidget::OnMouseEnter()
	{
		myImageCurrent = myImageHover;
	}

	void ButtonWidget::OnMouseExit()
	{
		myImageCurrent = myImageNormal;
	}

	bool ButtonWidget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return aPosition.x >= myPosition.x - myImageCurrent->GetHotspot().x &&
			aPosition.x <= myPosition.x + mySize.x - myImageCurrent->GetHotspot().x &&
			aPosition.y >= myPosition.y - myImageCurrent->GetHotspot().y &&
			aPosition.y <= myPosition.y + mySize.y - myImageCurrent->GetHotspot().y;

	//	return	aPosition.x >= myPosition.x - mySize.x / 2.f &&
	//		aPosition.y >= myPosition.y - mySize.y / 2.f &&
	//		aPosition.x <= myPosition.x + mySize.x / 2.f &&
	//		aPosition.y <= myPosition.y + mySize.y / 2.f;
	}

	void ButtonWidget::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myPosition = { aPosition.x + myImageCurrent->GetHotspot().x, aPosition.y - myImageCurrent->GetHotspot().y };
	}


	void ButtonWidget::Click()
	{

	}
}