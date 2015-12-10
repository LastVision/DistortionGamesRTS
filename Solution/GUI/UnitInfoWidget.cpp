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
		std::string gruntUnitPath = "";
		std::string gruntPortraitPath = "";
		std::string buildingPortraitPath = "";
		CU::Vector2<float> size;
		CU::Vector2<float> unitSize;
		CU::Vector2<float> portraitSize;
		CU::Vector2<float> portraitPosition;
		CU::Vector2<float> unitPosition;
		
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "x", unitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "y", unitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "x", portraitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "y", portraitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "x", unitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "y", unitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "x", portraitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "y", portraitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntunit"), "path", gruntUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntportrait"), "path", gruntPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buildingportrait"), "path", buildingPortraitPath);

		mySize = size;
		myPosition = { 0.f, 0.f };
		myUnitPosition = unitPosition;
		myPortraitPosition = portraitPosition;
		myGruntUnit = new Prism::Sprite(gruntUnitPath, unitSize);
		myGruntPortrait = new Prism::Sprite(gruntPortraitPath, portraitSize);
		myBuildingPortrait = new Prism::Sprite(buildingPortraitPath, portraitSize);
		myBuildingTimer = new BarWidget(myBuilding.GetMaxBuildTime(), myBuilding.GetCurrentBuildTime(), { unitSize.x * 4.f, unitSize.y / 2.f });
	}

	UnitInfoWidget::~UnitInfoWidget()
	{
		SAFE_DELETE(myGruntPortrait);
		SAFE_DELETE(myGruntUnit);
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
				if (myUnits.Size() > 1)
				{
					for (int i = 0; i < myUnits.Size(); i++)
					{
						CU::Vector2<float> position = { myPosition.x + myGruntUnit->GetSize().x * i, mySize.y / 2.f };
						position += aParentPosition + myUnitPosition;
						myGruntUnit->Render(position);
						// render health
					}
				}
				else
				{
					myGruntPortrait->Render(myPosition + aParentPosition + myPortraitPosition);
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				myBuildingPortrait->Render(myPosition + aParentPosition + myPortraitPosition);

				if (myBuilding.GetEntityToSpawn() != eEntityType::EMPTY)
				{
					myBuildingTimer->Update();
					CU::Vector2<float> position = myPosition + aParentPosition;
					position.x += myBuildingPortrait->GetSize().x * 2.f;
					position.y += myBuildingPortrait->GetSize().y / 2.f;
					myBuildingTimer->Render(position);
					position.y += myBuildingTimer->GetSize().y + 10.f;
					Prism::Engine::GetInstance()->PrintText(myBuilding.GetSpawnQueueSize(), position, Prism::eTextType::RELEASE_TEXT);
				}
			}
		}
	}

	void UnitInfoWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		myBuildingTimer->OnResize(aNewWindowSize, anOldWindowSize);

		CU::Vector2<float> unitRatioSize = myGruntUnit->GetSize() / anOldWindowSize;
		CU::Vector2<float> unitNewSize = unitRatioSize * aNewWindowSize;
		CU::Vector2<float> portraitRatioSize = myBuildingPortrait->GetSize() / anOldWindowSize;
		CU::Vector2<float> portraitNewSize = portraitRatioSize * aNewWindowSize;

		CU::Vector2<float> ratioUnitPostion = myUnitPosition / anOldWindowSize;
		CU::Vector2<float> ratioPortraitPostion = myPortraitPosition / anOldWindowSize;

		myUnitPosition = ratioUnitPostion * aNewWindowSize;
		myPortraitPosition = ratioPortraitPostion * aNewWindowSize;

		myGruntUnit->SetSize(unitNewSize, { 0.f, 0.f });
		myGruntPortrait->SetSize(portraitNewSize, { 0.f, 0.f });
		myBuildingPortrait->SetSize(portraitNewSize, { 0.f, 0.f });
	}
}