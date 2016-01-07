#include "stdafx.h"
#include "ButtonWidget.h"
#include "UpgradeButtonWidget.h"

namespace GUI
{
	UpgradeButtonWidget::UpgradeButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement
		, const int& aUpgradeLevel)
		: myUpgradeLevel(aUpgradeLevel)
	{
		int index = 0;

		tinyxml2::XMLElement* buttonElement = aReader->ForceFindFirstChild(anXMLElement, "widget");
		for (; buttonElement != nullptr; buttonElement = aReader->FindNextElement(buttonElement))
		{
			myUpgrades[index] = new ButtonWidget(aReader, buttonElement);
			myPosition = myUpgrades[index]->GetPosition();
			myUpgrades[index]->SetPosition(myUpgrades[index]->GetSize() / 2.f);
			index++;
		}

		mySize = myUpgrades[0]->GetSize();
		myPosition.x -= myUpgrades[0]->GetSize().x / 2.f;
	}

	UpgradeButtonWidget::~UpgradeButtonWidget()
	{
		for (int i = 0; i < AMOUNT_OF_UPGRADES; i++)
		{
			SAFE_DELETE(myUpgrades[i]);
		}
	}

	void UpgradeButtonWidget::Update()
	{

	}

	void UpgradeButtonWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myUpgrades[myUpgradeLevel]->Render(myPosition + aParentPosition);
	}

	Widget* UpgradeButtonWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (myUpgrades[myUpgradeLevel]->IsInside(aPosition - myPosition) == true)
		{
			return myUpgrades[myUpgradeLevel];
		}
		return this;
	}

	void UpgradeButtonWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);

		for (int i = 0; i < AMOUNT_OF_UPGRADES; i++)
		{
			myUpgrades[i]->OnResize(aNewSize, anOldSize);
		}
	}
}