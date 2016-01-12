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

		aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "cooldown"), "type", type);

		if (type == "totem")
		{
			myCooldown = &aPlayer->GetTotemCooldown();
		}
		//else
		//{
		//	DL_ASSERT("[AbilityButton] Cooldown type wrong");
		//}
	}

	AbilityButton::~AbilityButton()
	{
	}

	void AbilityButton::Render(const CU::Vector2<float>& aParentPosition)
	{
		ButtonWidget::Render(aParentPosition);

		if (*myCooldown > 0.f)
		{
			Prism::Engine::GetInstance()->PrintText(*myCooldown, aParentPosition, Prism::eTextType::RELEASE_TEXT);
		}
	}
}