#include "stdafx.h"
#include "ButtonWidget.h"
#include "../Entity/Entity.h"
#include "UnitActionWidget.h"
#include "WidgetContainer.h"

namespace GUI
{
	UnitActionWidget::UnitActionWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits)
		: myUnits(someUnits)
		, myUnitActionButtons(nullptr)
		, myBuildingActionButtons(nullptr)
		, myIsUnitSelected(false)
		, myHasSelectedGrunt(false)
		, myHasSelectedRanger(false)
		, myHasSelectedTank(false)
		, mySelectedType(eEntityType::_COUNT)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;
		CU::Vector2<float> buttonSize;
		CU::Vector2<float> buttonPosition;
		std::string path = "";

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

		myGruntActionButtons = ReadContainer(aReader, gruntElement, { buttonSize.x, buttonSize.y * 2.f });
		myRangerActionButtons = ReadContainer(aReader, rangerElement, { buttonSize.x, buttonSize.y * 2.f });
		myTankActionButtons = ReadContainer(aReader, tankElement, { buttonSize.x, buttonSize.y * 2.f });

		myGruntActionButtons->SetPosition({ buttonPosition.x + (buttonSize.x * 2.f) + 1.f, (buttonPosition.y - buttonSize.y * 3.f) - 3.f });
		myRangerActionButtons->SetPosition({ buttonPosition.x + (buttonSize.x * 3.f) + 2.f, (buttonPosition.y - buttonSize.y * 3.f) - 3.f });
		myTankActionButtons->SetPosition({ buttonPosition.x + (buttonSize.x * 4.f) + 3.f, (buttonPosition.y - buttonSize.y * 3.f) - 3.f });
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

				if (myHasSelectedGrunt == true)
				{
					myGruntActionButtons->Render(myPosition + aParentPosition);
				}

				if (myHasSelectedRanger == true)
				{
					myRangerActionButtons->Render(myPosition + aParentPosition);
				}

				if (myHasSelectedTank == true)
				{
					myTankActionButtons->Render(myPosition + aParentPosition);
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				myBuildingActionButtons->Render(myPosition + aParentPosition);
			}
		}
	}

	void UnitActionWidget::Update(float)
	{
		myHasSelectedGrunt = false;
		myHasSelectedRanger = false;
		myHasSelectedTank = false;

		myIsUnitSelected = myUnits.Size() > 0;
		if (myIsUnitSelected == true)
		{
			mySelectedType = myUnits[0]->GetType();
		}

		if (mySelectedType == eEntityType::UNIT)
		{
			for (int i = 0; i < myUnits.Size(); i++)
			{
				if (myHasSelectedGrunt == false && myUnits[i]->GetUnitType() == eUnitType::GRUNT)
				{
					myHasSelectedGrunt = true;
				}

				if (myHasSelectedRanger == false && myUnits[i]->GetUnitType() == eUnitType::RANGER)
				{
					myHasSelectedRanger = true;
				}

				if (myHasSelectedTank == false && myUnits[i]->GetUnitType() == eUnitType::TANK)
				{
					myHasSelectedTank = true;
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

				if (myHasSelectedGrunt == true)
				{
					Widget* gruntWidget = myGruntActionButtons->MouseIsOver(aPosition - myPosition);
					widget = gruntWidget == nullptr ? widget : gruntWidget;
				}
				if(myHasSelectedRanger == true)
				{
					Widget* rangerWidget = myRangerActionButtons->MouseIsOver(aPosition - myPosition);
					widget = rangerWidget == nullptr ? widget : rangerWidget;
				}
				if (myHasSelectedTank == true)
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
				return myBuildingActionButtons->MouseIsOver(aPosition - myPosition);
			}
		}
		return nullptr;
	}

	void UnitActionWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		myUnitActionButtons->OnResize(aNewWindowSize, anOldWindowSize);
		myBuildingActionButtons->OnResize(aNewWindowSize, anOldWindowSize);
		myGruntActionButtons->OnResize(aNewWindowSize, anOldWindowSize);
		myRangerActionButtons->OnResize(aNewWindowSize, anOldWindowSize);
		myTankActionButtons->OnResize(aNewWindowSize, anOldWindowSize);
	}

	WidgetContainer* UnitActionWidget::ReadContainer(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, CU::Vector2<float> aSize)
	{
		GUI::WidgetContainer* container = new WidgetContainer(nullptr, aSize);
		tinyxml2::XMLElement* widgetElement = aReader->FindFirstChild(anXMLElement, "widget");
		for (; widgetElement != nullptr; widgetElement = aReader->FindNextElement(widgetElement))
		{
			std::string type = "";

			aReader->ForceReadAttribute(widgetElement, "type", type);

			if (type == "button")
			{
				ButtonWidget* button = new ButtonWidget(aReader, widgetElement);
				container->AddWidget(button);
			}
		}
		return container;
	}
}