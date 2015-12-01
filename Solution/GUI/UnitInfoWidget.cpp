#include "stdafx.h"
#include "UnitInfoWidget.h"
#include "WidgetContainer.h"
#include <Sprite.h>
#include <XMLReader.h>
#include "../Entity/Entity.h"

namespace GUI
{
	UnitInfoWidget::UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits)
		: myUnits(someUnits)
	{
		std::string unitPath = "";
		CU::Vector2<float> size;
		CU::Vector2<float> unitSize;
		CU::Vector2<float> position;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "x", unitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "y", unitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "testunitsprite"), "path", unitPath);

		mySize = size;
		myPosition = position;
		myUnitPortrait = new Prism::Sprite(unitPath, unitSize);

	}

	UnitInfoWidget::~UnitInfoWidget()
	{
		SAFE_DELETE(myUnitPortrait);
	}

	void UnitInfoWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		for (int i = 0; i < myUnits.Size(); i++)
		{
			if (myUnits[i] != nullptr && myUnits[i]->IsSelected() == true)
			{
				CU::Vector2<float> position = { myPosition.x + myUnitPortrait->GetSize().x * i, myPosition.y };
				position += aParentPosition;
				myUnitPortrait->Render(position);
				// render health
			}
		}
	}

	void UnitInfoWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);

		CU::Vector2<float> ratioSize = myUnitPortrait->GetSize() / anOldWindowSize;
		CU::Vector2<float> newSize = ratioSize * aNewWindowSize;

		myUnitPortrait->SetSize(newSize, newSize / 2.f);
	}
}