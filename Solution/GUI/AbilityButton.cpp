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
		, myPercentageMultiplier(0.f)
	{
		std::string type = "";
		std::string outline = "";

		tinyxml2::XMLElement* cooldownElement = aReader->FindFirstChild(anXMLElement, "cooldown");

		if (cooldownElement != nullptr)
		{
			aReader->ReadAttribute(cooldownElement, "type", type);
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

		aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "outlinesprite"), "path", outline);
		aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "outlinesprite"), "percentagemultiplier", myPercentageMultiplier);

		myCooldownIndicator = Prism::ModelLoader::GetInstance()->LoadSprite(outline, mySize);
		myHasCooldownSprite = Prism::ModelLoader::GetInstance()->LoadSprite(outline, mySize);
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
			percentage += (1.f - percentage) * myPercentageMultiplier;
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