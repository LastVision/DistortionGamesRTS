#include "stdafx.h"
#include "ButtonWidget.h"
#include "UnitActionWidget.h"
#include <Sprite.h>
#include <XMLReader.h>
#include "WidgetContainer.h"

namespace GUI
{
	UnitActionWidget::UnitActionWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits)
		: myUnits(someUnits)
		, myUnitActionButtons(nullptr)
		, myBuildingActionButtons(nullptr)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;
		std::string path = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsprite"), "path", path);

		mySize = size;
		myPosition = position;

		tinyxml2::XMLElement* containerElement = aReader->ForceFindFirstChild(anXMLElement, "container");
		//for (; containerElement != nullptr; containerElement = aReader->FindNextElement(containerElement))
		//{
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
		//}
	}

	UnitActionWidget::~UnitActionWidget()
	{
		SAFE_DELETE(myUnitActionButtons);
		SAFE_DELETE(myBuildingActionButtons);
	}

	void UnitActionWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myUnitActionButtons->Render(myPosition + aParentPosition);
	}

	void UnitActionWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		myUnitActionButtons->OnResize(aNewWindowSize, anOldWindowSize);
	}
}