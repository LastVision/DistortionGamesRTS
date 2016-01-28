#include "stdafx.h"
#include "AbilityButton.h"
#include "ButtonWidget.h"
#include "../Entity/Entity.h"
#include "../Entity/EnrageComponent.h"
#include "../Entity/GrenadeComponent.h"
#include "../Entity/PromotionComponent.h"
#include "../Game/PlayerDirector.h"
#include "UnitActionWidget.h"
#include "UpgradeButtonWidget.h"
#include "WidgetContainer.h"
#include "../Entity/ActorComponent.h"

namespace GUI
{
	UnitActionWidget::UnitActionWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits
		, const PlayerDirector* aPlayer)
		: Widget()
		, myUnits(someUnits)
		, myUnitActionButtons(nullptr)
		, myBuildingActionButtons(nullptr)
		, myIsUnitSelected(false)
		, mySelectedType(eEntityType::_COUNT)
		, myPlayer(aPlayer)
		, myHasUnlockedRanger(aPlayer->HasUnlockedRanger())
		, myHasUnlockedTank(aPlayer->HasUnlockedTank())
		, myFirstSelectedGrunt(nullptr)
		, myFirstSelectedRanger(nullptr)
		, myFirstSelectedTank(nullptr)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;
		CU::Vector2<float> buttonSize;
		CU::Vector2<float> buttonPosition;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);

		mySize = size;
		myPosition = position;

		tinyxml2::XMLElement* unitElement = aReader->ForceFindFirstChild(anXMLElement, "unit");
		tinyxml2::XMLElement* gruntElement = aReader->ForceFindFirstChild(anXMLElement, "grunt");
		tinyxml2::XMLElement* rangerElement = aReader->ForceFindFirstChild(anXMLElement, "ranger");
		tinyxml2::XMLElement* tankElement = aReader->ForceFindFirstChild(anXMLElement, "tank");
		tinyxml2::XMLElement* buildingElement = aReader->ForceFindFirstChild(anXMLElement, "building");

		myUnitActionButtons = ReadContainer(aReader, unitElement, size);
		myBuildingActionButtons = ReadContainer(aReader, buildingElement, size);

		buttonSize = myUnitActionButtons->GetFirstWidget()->GetSize();
		buttonPosition = myUnitActionButtons->GetFirstWidget()->GetPosition();

		buttonPosition.x -= (buttonSize.x * 0.5f);
		buttonPosition.y += (buttonSize.y * 0.5f);

		myGruntActionButtons = ReadContainer(aReader, gruntElement, buttonSize);
		myRangerActionButtons = ReadContainer(aReader, rangerElement, buttonSize);
		myTankActionButtons = ReadContainer(aReader, tankElement, buttonSize);

		myGruntActionButtons->SetPosition({ buttonPosition.x, (buttonPosition.y - buttonSize.y * 2.f) - 2.f });
		myRangerActionButtons->SetPosition({ buttonPosition.x + (buttonSize.x * 1.f) + 1.f, (buttonPosition.y - buttonSize.y * 2.f) - 2.f });
		myTankActionButtons->SetPosition({ buttonPosition.x + (buttonSize.x * 2.f) + 2.f, (buttonPosition.y - buttonSize.y * 2.f) - 2.f });
	}

	UnitActionWidget::~UnitActionWidget()
	{
		SAFE_DELETE(myUnitActionButtons);
		SAFE_DELETE(myGruntActionButtons);
		SAFE_DELETE(myRangerActionButtons);
		SAFE_DELETE(myTankActionButtons);
		SAFE_DELETE(myBuildingActionButtons);
	}

	void UnitActionWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
				myUnitActionButtons->Render(myPosition + aParentPosition);

				if (myFirstSelectedGrunt != nullptr && myFirstSelectedGrunt->GetComponent<PromotionComponent>()->GetPromoted() == true)
				{
					myGruntActionButtons->Render(myPosition + aParentPosition);
				}

				if (myFirstSelectedRanger != nullptr && myFirstSelectedRanger->GetComponent<PromotionComponent>()->GetPromoted() == true)
				{
					myRangerActionButtons->Render(myPosition + aParentPosition);
				}

				if (myFirstSelectedTank != nullptr && myFirstSelectedTank->GetComponent<PromotionComponent>()->GetPromoted() == true)
				{
					myTankActionButtons->Render(myPosition + aParentPosition);
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				myBuildingActionButtons->At(0)->Render(myPosition + aParentPosition);
				myBuildingActionButtons->At(3)->Render(myPosition + aParentPosition);

				if (myHasUnlockedRanger == true)
				{
					myBuildingActionButtons->At(1)->Render(myPosition + aParentPosition);
					myBuildingActionButtons->At(4)->Render(myPosition + aParentPosition);

				}
				if (myHasUnlockedTank == true)
				{
					myBuildingActionButtons->At(2)->Render(myPosition + aParentPosition);
					myBuildingActionButtons->At(5)->Render(myPosition + aParentPosition);
				}
			}
		}
	}

	void UnitActionWidget::Update(float)
	{
		myFirstSelectedGrunt = nullptr;
		myFirstSelectedRanger = nullptr;
		myFirstSelectedTank = nullptr;

		myIsUnitSelected = myUnits.Size() > 0;
		if (myIsUnitSelected == true)
		{
			mySelectedType = myUnits[0]->GetType();
		}

		if (mySelectedType == eEntityType::UNIT)
		{
			for (int i = 0; i < myUnits.Size(); i++)
			{
				if (myFirstSelectedGrunt == nullptr && myUnits[i]->GetUnitType() == eUnitType::GRUNT)
				{
					myGruntActionButtons->At(0)->SetValue(myUnits[i]->GetComponent<GrenadeComponent>()->GetCooldown(), myUnits[i]->GetComponent<GrenadeComponent>()->GetMaxCooldown());
					myFirstSelectedGrunt = myUnits[i];
				}

				if (myFirstSelectedRanger == nullptr && myUnits[i]->GetUnitType() == eUnitType::RANGER)
				{
					myRangerActionButtons->At(0)->SetValue(myUnits[i]->GetComponent<ActorComponent>()->GetRangerCooldown(), myUnits[i]->GetComponent<ActorComponent>()->GetRangerMaxCooldown());
					myFirstSelectedRanger = myUnits[i];
				}

				if (myFirstSelectedTank == nullptr && myUnits[i]->GetUnitType() == eUnitType::TANK)
				{
					myTankActionButtons->At(0)->SetValue(myUnits[i]->GetComponent<EnrageComponent>()->GetCooldown(), myUnits[i]->GetComponent<EnrageComponent>()->GetMaxCooldown());
					myFirstSelectedTank = myUnits[i];
				}
			}
		}
	}

	Widget* UnitActionWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
				Widget* widget = nullptr;

				if (myFirstSelectedGrunt != nullptr && myFirstSelectedGrunt->GetComponent<PromotionComponent>()->GetPromoted() == true)
				{
					Widget* gruntWidget = myGruntActionButtons->MouseIsOver(aPosition - myPosition);
					widget = gruntWidget == nullptr ? widget : gruntWidget;
				}
				if (myFirstSelectedRanger != nullptr && myFirstSelectedRanger->GetComponent<PromotionComponent>()->GetPromoted() == true)
				{
					Widget* rangerWidget = myRangerActionButtons->MouseIsOver(aPosition - myPosition);
					widget = rangerWidget == nullptr ? widget : rangerWidget;
				}
				if (myFirstSelectedTank != nullptr && myFirstSelectedTank->GetComponent<PromotionComponent>()->GetPromoted() == true)
				{
					Widget* tankWidget = myTankActionButtons->MouseIsOver(aPosition - myPosition);
					widget = tankWidget == nullptr ? widget : tankWidget;
				}

				if (widget != nullptr)
				{
					return widget;
				}

				return myUnitActionButtons->MouseIsOver(aPosition - myPosition);
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				Widget* widget = myBuildingActionButtons->MouseIsOver(aPosition - myPosition);

				if (myHasUnlockedRanger == false && (widget == myBuildingActionButtons->At(1) || widget == myBuildingActionButtons->At(4)))
				{
					return nullptr;
				}
				else if (myHasUnlockedTank == false && (widget == myBuildingActionButtons->At(2) || widget == myBuildingActionButtons->At(5)))
				{
					return nullptr;
				}

				return widget;
			}
		}
		return nullptr;
	}

	void UnitActionWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myUnitActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myBuildingActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myGruntActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myRangerActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myTankActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
	}

	WidgetContainer* UnitActionWidget::ReadContainer(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, CU::Vector2<float> aSize)
	{
		GUI::WidgetContainer* container = new WidgetContainer(nullptr, aSize, false);
		tinyxml2::XMLElement* widgetElement = aReader->FindFirstChild(anXMLElement, "widget");
		for (; widgetElement != nullptr; widgetElement = aReader->FindNextElement(widgetElement))
		{
			std::string type = "";

			aReader->ForceReadAttribute(widgetElement, "type", type);

			if (type == "button")
			{
				ButtonWidget* button = new ButtonWidget(aReader, widgetElement, myPlayer);
				container->AddWidget(button);
			}
			else if (type == "upgrade_button")
			{
				UpgradeButtonWidget* upgradeButtonWidget = new UpgradeButtonWidget(aReader, widgetElement, myPlayer);
				container->AddWidget(upgradeButtonWidget);
			}
			else if (type == "ability_button")
			{
				AbilityButton* upgradeButtonWidget = new AbilityButton(aReader, widgetElement, myPlayer);
				container->AddWidget(upgradeButtonWidget);
			}
		}
		return container;
	}
}