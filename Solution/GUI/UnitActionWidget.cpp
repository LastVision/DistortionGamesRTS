#include "stdafx.h"
#include "ButtonWidget.h"
#include "../Entity/Entity.h"
#include <Sprite.h>
#include "UnitActionWidget.h"
#include "WidgetContainer.h"
#include <XMLReader.h>

namespace GUI
{
	UnitActionWidget::UnitActionWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits)
		: myUnits(someUnits)
		, myUnitActionButtons(nullptr)
		, myBuildingActionButtons(nullptr)
		, myIsUnitSelected(false)
		, mySelectedType(eEntityType::_COUNT)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;
		std::string path = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);

		mySize = size;
		myPosition = position;

		tinyxml2::XMLElement* containerElement = aReader->ForceFindFirstChild(anXMLElement, "unit");
		GUI::WidgetContainer* container = new WidgetContainer(nullptr, size);
		tinyxml2::XMLElement* widgetElement = aReader->FindFirstChild(containerElement, "widget");
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
		myUnitActionButtons = container;

		tinyxml2::XMLElement* buildingElement = aReader->ForceFindFirstChild(anXMLElement, "building");
		GUI::WidgetContainer* buildingContainer = new WidgetContainer(nullptr, size);
		tinyxml2::XMLElement* buildingWidgetElement = aReader->FindFirstChild(buildingElement, "widget");
		for (; buildingWidgetElement != nullptr; buildingWidgetElement = aReader->FindNextElement(buildingWidgetElement))
		{
			std::string type = "";

			aReader->ForceReadAttribute(buildingWidgetElement, "type", type);

			if (type == "button")
			{
				ButtonWidget* button = new ButtonWidget(aReader, buildingWidgetElement);
				buildingContainer->AddWidget(button);
			}
		}
		myBuildingActionButtons = buildingContainer;
	}

	UnitActionWidget::~UnitActionWidget()
	{
		SAFE_DELETE(myUnitActionButtons);
		SAFE_DELETE(myBuildingActionButtons);
	}

	void UnitActionWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
				myUnitActionButtons->Render(myPosition + aParentPosition);
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				myBuildingActionButtons->Render(myPosition + aParentPosition);
			}
		}
	}

	void UnitActionWidget::Update()
	{
		Widget::Update();

		myIsUnitSelected = myUnits.Size() > 0;
		if (myIsUnitSelected == true)
		{
			mySelectedType = myUnits[0]->GetType();
		}
	}

	Widget* UnitActionWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
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
	}
}