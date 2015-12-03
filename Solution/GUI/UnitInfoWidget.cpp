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
		, mySelectedType(eEntityType::_COUNT)
	{
		std::string unitPath = "";
		std::string buildingPath = "";
		CU::Vector2<float> size;
		CU::Vector2<float> unitSize;
		CU::Vector2<float> portraitSize;
		CU::Vector2<float> position;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "x", unitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "y", unitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "x", portraitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "y", portraitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "testunitsprite"), "path", unitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "testbuildingsprite"), "path", buildingPath);

		mySize = size;
		myPosition = position;
		myUnitPortrait = new Prism::Sprite(unitPath, unitSize);
		myBuildingPortrait = new Prism::Sprite(buildingPath, portraitSize);
	}

	UnitInfoWidget::~UnitInfoWidget()
	{
		SAFE_DELETE(myUnitPortrait);
		SAFE_DELETE(myBuildingPortrait);
	}

	void UnitInfoWidget::Update()
	{
		myIsUnitSelected = myUnits.Size() > 0;
		if (myIsUnitSelected == true)
		{
			mySelectedType = myUnits[0]->GetType();
		}
	}

	void UnitInfoWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::DRAGON)
			{
				for (int i = 0; i < myUnits.Size(); i++)
				{
					CU::Vector2<float> position = { myPosition.x + myUnitPortrait->GetSize().x * i, mySize.y / 2.f };
					position += aParentPosition;
					myUnitPortrait->Render(position);
					// render health
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				myBuildingPortrait->Render(myPosition + aParentPosition);
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