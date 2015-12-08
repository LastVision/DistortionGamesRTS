#include "stdafx.h"
#include "BarWidget.h"
#include <Engine.h>
#include "../Entity/BuildingComponent.h"
#include "../Entity/Entity.h"
#include "../Game/PlayerDirector.h"
#include "UnitInfoWidget.h"
#include "WidgetContainer.h"
#include <Sprite.h>
#include <XMLReader.h>

namespace GUI
{
	UnitInfoWidget::UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: myUnits(aPlayer->GetSelectedUnits())
		, mySelectedType(eEntityType::_COUNT)
		, myBuilding(aPlayer->GetBuildingComponent())
		, myBuildingTimer(nullptr)
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
		myBuildingTimer = new BarWidget(myBuilding.GetMaxBuildTime(), myBuilding.GetCurrentBuildTime(), { unitSize.x * 4.f, unitSize.y / 2.f });
	}

	UnitInfoWidget::~UnitInfoWidget()
	{
		SAFE_DELETE(myUnitPortrait);
		SAFE_DELETE(myBuildingPortrait);
		SAFE_DELETE(myBuildingTimer);
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

				if (myBuilding.GetEntityToSpawn() != eEntityType::EMPTY)
				{
					myBuildingTimer->Update();
					CU::Vector2<float> position = myPosition + aParentPosition;
					position.x += myBuildingPortrait->GetSize().x * 2.f;
					position.y += myBuildingPortrait->GetSize().y / 2.f;
					myBuildingTimer->Render(position);
				}
			}
		}
	}

	void UnitInfoWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		myBuildingTimer->OnResize(aNewWindowSize, anOldWindowSize);

		CU::Vector2<float> unitRatioSize = myUnitPortrait->GetSize() / anOldWindowSize;
		CU::Vector2<float> unitNewSize = unitRatioSize * aNewWindowSize;
		CU::Vector2<float> buildingRatioSize = myBuildingPortrait->GetSize() / anOldWindowSize;
		CU::Vector2<float> buildingNewSize = buildingRatioSize * aNewWindowSize;

		myUnitPortrait->SetSize(unitNewSize, { 0.f, 0.f });
		myBuildingPortrait->SetSize(buildingNewSize, { 0.f, 0.f });
	}
}