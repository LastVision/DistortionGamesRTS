#include "stdafx.h"
#include "ButtonWidget.h"
#include <OnClickMessage.h>
#include <Sprite.h>
#include <PostMaster.h>
#include <XMLReader.h>

namespace GUI
{
	ButtonWidget::ButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
		, myClickEvent(nullptr)
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

		if (aReader->FindFirstChild(anXMLElement, "hover") != nullptr)
		{
			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "hover"), "text", hoverText);
		}

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "event", clickEvent);

		if (clickEvent == "lose")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::LOSE);
		}
		else if (clickEvent == "win")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::WIN);
		}
		else if (clickEvent == "start")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::START);
		}
		else if (clickEvent == "restart")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::RESTART);
		}
		else if (clickEvent == "quit")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::QUIT);
		}
		else if (clickEvent == "unit")
		{
			int ID = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", ID);
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT, ID);
		}
		else
		{
			std::string message = "[GUI]: No onclick event named " + clickEvent;
			DL_ASSERT(message);
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
		SAFE_DELETE(myImageNormal);
		SAFE_DELETE(myImagePressed);
		SAFE_DELETE(myImageHover);
		SAFE_DELETE(myClickEvent);
		myImageCurrent = nullptr;
	}

	void ButtonWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myImageCurrent->Render(myPosition + aParentPosition);
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

	void ButtonWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myImageNormal->SetSize(mySize, mySize / 2.f);
		myImagePressed->SetSize(mySize, mySize / 2.f);
		myImageHover->SetSize(mySize, mySize / 2.f);
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
		if (myClickEvent != nullptr)
		{
			PostMaster::GetInstance()->SendMessage(*myClickEvent);
		}
	}
}