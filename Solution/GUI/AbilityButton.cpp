#include "stdafx.h"
#include "AbilityButton.h"
#include <Engine.h>
#include "../Game/PlayerDirector.h"
#include <OnClickMessage.h>

namespace GUI
{
	AbilityButton::AbilityButton(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: ButtonWidget(aReader, anXMLElement)
		, myCooldown(nullptr)
		, myMaxCooldown(nullptr)
		, myCooldownIndicator(nullptr)
		, myHasCooldownSprite(nullptr)
	{
		std::string type = "";

		tinyxml2::XMLElement* XMLElement = aReader->FindFirstChild(anXMLElement, "cooldown");

		if (XMLElement != nullptr)
		{
			aReader->ReadAttribute(XMLElement, "type", type);
			if (type == "totem")
			{
				myCooldown = &aPlayer->GetTotemCooldown();
				myMaxCooldown = &aPlayer->GetTotemMaxCooldown();
			}
			else if (type == "upgrade")
			{
				myCooldown = &aPlayer->GetUpgradeCooldown(myClickEvent->myID);
				myMaxCooldown = &aPlayer->GetUpgradeMaxCooldown(myClickEvent->myID);
			}
		}

		myCooldownIndicator = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_outline_button.dds", mySize);
		myHasCooldownSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_outline_button.dds", mySize);
	}

	AbilityButton::~AbilityButton()
	{
		SAFE_DELETE(myCooldownIndicator);
		SAFE_DELETE(myHasCooldownSprite);
	}

	void AbilityButton::Render(const CU::Vector2<float>& aParentPosition)
	{
		ButtonWidget::Render(aParentPosition);

		if (myMaxCooldown != nullptr && myCooldown != nullptr && *myCooldown > 0.f)
		{
			CU::Vector2<float> size = mySize;
			CU::Vector2<float> position = aParentPosition;
			float percentage = (*myCooldown / *myMaxCooldown);
			percentage += (1.f - percentage) * 0.1f;
			size.y *= percentage;


			myCooldownIndicator->SetSize(size, { 0.f, 0.f });
			myCooldownIndicator->SetUVZeroToOne({ 0.f, 1.f - percentage }, { 1.f, 1.f });

			myHasCooldownSprite->Render(aParentPosition, { 1.f, 1.f }, { 0.f, 0.f, 0.f, 0.5f });
			myCooldownIndicator->Render(aParentPosition, { 1.f, 1.f }, { 1.f, 1.f, 1.f, 0.5f });
			Prism::Engine::GetInstance()->PrintText(*myCooldown, aParentPosition, Prism::eTextType::DEBUG_TEXT);
		}
	}

	void AbilityButton::SetValue(const float& aFirstValue, const float& aSecondValue)
	{
		myCooldown = &aFirstValue;
		myMaxCooldown = &aSecondValue;
	}
}