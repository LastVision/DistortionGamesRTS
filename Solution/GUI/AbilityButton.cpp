#include "stdafx.h"
#include "AbilityButton.h"
#include <Engine.h>
#include "../Game/PlayerDirector.h"

namespace GUI
{
	AbilityButton::AbilityButton(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: ButtonWidget(aReader, anXMLElement)
		, myCooldown(nullptr)
	{
		std::string type = "";

		tinyxml2::XMLElement* XMLElement = aReader->FindFirstChild(anXMLElement, "cooldown");

		if (XMLElement != nullptr)
		{
			aReader->ReadAttribute(XMLElement, "type", type);
			if (type == "totem")
			{
				myCooldown = &aPlayer->GetTotemCooldown();
			}
		}
	}

	AbilityButton::~AbilityButton()
	{
	}

	void AbilityButton::Render(const CU::Vector2<float>& aParentPosition)
	{
		ButtonWidget::Render(aParentPosition);

		if (myCooldown != nullptr && *myCooldown > 0.f)
		{
			Prism::Engine::GetInstance()->PrintText(*myCooldown, aParentPosition, Prism::eTextType::RELEASE_TEXT);
		}
	}

	void AbilityButton::SetValue(const float& aValue)
	{
		myCooldown = &aValue;
	}
}