#include "stdafx.h"

#include <AudioInterface.h>
#include "ButtonWidget.h"
#include <Engine.h>
#include <OnClickMessage.h>
#include <PostMaster.h>

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

		ReadEvent(aReader, anXMLElement);

		mySize = size;
		myPosition = position;
		myHoverText = hoverText;

		myImageNormal = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathNormal, mySize, mySize / 2.f);
		myImageHover = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathHover, mySize, mySize / 2.f);
		myImagePressed = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathPressed, mySize, mySize / 2.f);
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

		if (myImageCurrent == myImageHover && myHoverText != "")
		{
			CU::Vector2<float> hoverPosition = { myPosition.x - mySize.x / 2.f
				, myPosition.y + mySize.y / 2.f };
			hoverPosition += aParentPosition;

			Prism::Engine::GetInstance()->PrintText(myHoverText, hoverPosition, Prism::eTextType::RELEASE_TEXT);
		}
	}

	void ButtonWidget::OnMousePressed(const CU::Vector2<float>&)
	{
		myImageCurrent = myImagePressed;
	}

	void ButtonWidget::OnMouseUp()
	{
		Click();
		myImageCurrent = myImageNormal;
	}

	void ButtonWidget::OnMouseEnter()
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("buttonHover", 0);
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
	}

	void ButtonWidget::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myPosition = { aPosition.x + myImageCurrent->GetHotspot().x, aPosition.y - myImageCurrent->GetHotspot().y };
	}

	void ButtonWidget::ReadEvent(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
	{
		std::string clickEvent = "";
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "event", clickEvent);

		//if (clickEvent == "game_lose")
		//{
		//	myClickEvent = new OnClickMessage(eOnClickEvent::GAME_LOSE);
		//}
		//else if (clickEvent == "game_win")
		//{
		//	myClickEvent = new OnClickMessage(eOnClickEvent::GAME_WIN);
		//}
		if (clickEvent == "game_start")
		{
			int id = 0;
			aReader->ReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", id);
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_START, id);
		}
		else if (clickEvent == "game_level_select")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_LEVEL_SELECT);
		}
		else if (clickEvent == "game_restart")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_RESTART);
		}
		else if (clickEvent == "game_quit")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_QUIT);
		}
		else if (clickEvent == "action_move")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_MOVE);
		}
		else if (clickEvent == "action_attack")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_ATTACK);
		}
		else if (clickEvent == "action_stop")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_STOP);
		}
		else if (clickEvent == "action_place_totem")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::PLACE_TOTEM);
		}
		else if (clickEvent == "action_move_attack")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_ATTACK_MOVE);
		}
		else if (clickEvent == "action_stand_ground")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::UNIT_ACTION_STAND_GROUND);
		}
		else if (clickEvent == "action_enrage")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::ENRAGE);
		}
		else if (clickEvent == "spawn_unit")
		{
			int ID = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", ID);
			myClickEvent = new OnClickMessage(eOnClickEvent::SPAWN_UNIT, ID);
		}
		else if (clickEvent == "select_control_group")
		{
			int index = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", index);
			myClickEvent = new OnClickMessage(eOnClickEvent::SELECT_CONTROL_GROUP, index);
		}
		else if (clickEvent == "upgrade_unit")
		{
			int unitID = -1;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", unitID);
			myClickEvent = new OnClickMessage(eOnClickEvent::UPGRADE_UNIT, unitID);
		}
		else
		{
			std::string message = "[ButtonWidget]: No onclick event named " + clickEvent;
			DL_ASSERT(message);
		}
	}

	void ButtonWidget::Click()
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("buttonClick", 0);
		if (myClickEvent != nullptr)
		{
			PostMaster::GetInstance()->SendMessage(*myClickEvent);
		}
	}
}