#include "stdafx.h"
#include "BarWidget.h"
#include <Engine.h>
#include "../Entity/EnrageComponent.h"
#include "../Entity/BuildingComponent.h"
#include "../Entity/Entity.h"
#include "../Game/PlayerDirector.h"
#include "../Entity/ActorComponent.h"
#include "../Entity/ControllerComponent.h"
#include "../Entity/HealthComponent.h"
#include "../Entity/PromotionComponent.h"
#include <PostMaster.h>
#include <SelectUnitMessage.h>
#include "UnitInfoWidget.h"
#include "WidgetContainer.h"

namespace GUI
{
	UnitInfoWidget::UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: Widget()
		, myUnits(aPlayer->GetSelectedUnits())
		, mySelectedType(eEntityType::_COUNT)
		, myBuilding(aPlayer->GetBuildingComponent())
		, myBuildingTimer(nullptr)
		, myTextScale(1.f)
	{
		std::string gruntUnitPath = "";
		std::string rangerUnitPath = "";
		std::string tankUnitPath = "";
		std::string gruntPortraitPath = "";
		std::string rangerPortraitPath = "";
		std::string tankPortraitPath = "";
		std::string buildingPortraitPath = "";
		std::string statsSpritePath = "";
		CU::Vector2<float> unitSize;
		CU::Vector2<float> portraitSize;
		CU::Vector2<float> statsSize;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "x", unitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "y", unitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "x", portraitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "y", portraitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssize"), "x", statsSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssize"), "y", statsSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "x", myUnitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "y", myUnitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "x", myPortraitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "y", myPortraitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntunit"), "path", gruntUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "rangerunit"), "path", rangerUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "tankunit"), "path", tankUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntportrait"), "path", gruntPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "rangerportrait"), "path", rangerPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "tankportrait"), "path", tankPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buildingportrait"), "path", buildingPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssprite"), "path", statsSpritePath);

		myPosition = { 0.f, 0.f };
		myGruntUnit = Prism::ModelLoader::GetInstance()->LoadSprite(gruntUnitPath, unitSize);
		myRangerUnit = Prism::ModelLoader::GetInstance()->LoadSprite(rangerUnitPath, unitSize);
		myTankUnit = Prism::ModelLoader::GetInstance()->LoadSprite(tankUnitPath, unitSize);
		myGruntPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(gruntPortraitPath, portraitSize);
		myRangerPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(rangerPortraitPath, portraitSize);
		myTankPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(tankPortraitPath, portraitSize);
		myBuildingPortrait = Prism::ModelLoader::GetInstance()->LoadSprite(buildingPortraitPath, portraitSize);
		myStatsSprite = Prism::ModelLoader::GetInstance()->LoadSprite(statsSpritePath, statsSize);
		myBuildingTimer = new BarWidget(myBuilding.GetMaxBuildTime(), myBuilding.GetCurrentBuildTime(), { unitSize.x * 4.f, unitSize.y / 2.f }, { 0.f, 0.f, 1.f, 1.f });
	}

	UnitInfoWidget::~UnitInfoWidget()
	{
		SAFE_DELETE(myGruntUnit);
		SAFE_DELETE(myRangerUnit);
		SAFE_DELETE(myTankUnit);
		SAFE_DELETE(myGruntPortrait);
		SAFE_DELETE(myRangerPortrait);
		SAFE_DELETE(myTankPortrait);
		SAFE_DELETE(myBuildingPortrait);
		SAFE_DELETE(myStatsSprite);
		SAFE_DELETE(myBuildingTimer);
	}

	void UnitInfoWidget::Update(float)
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
			if (mySelectedType == eEntityType::UNIT)
			{
				if (myUnits.Size() > 1)
				{
					RenderUnitGroupInfo(aParentPosition);
				}
				else
				{
					RenderUnitInfo(aParentPosition);
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				RenderBaseInfo(aParentPosition);
			}
		}
	}

	void UnitInfoWidget::OnMousePressed(const CU::Vector2<float>& aPosition)
	{
		if (myUnits.Size() > 1)
		{
			for (int i = 0; i < myUnits.Size(); i++)
			{
				CU::Vector2<float> position = { (myGruntUnit->GetSize().x * i) + (i * 10.f), 0.f };
				position += myUnitPosition + myParent->GetPosition(); // hitta bättre lösning för detta

				if (aPosition.x >= position.x &&
					aPosition.y >= position.y &&
					aPosition.x <= position.x + myGruntUnit->GetSize().x &&
					aPosition.y <= position.y + myGruntUnit->GetSize().y)
				{
					PostMaster::GetInstance()->SendMessage(SelectUnitMessage(i));
				}
			}
		}
	}

	void UnitInfoWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myBuildingTimer->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myTextScale = (myTextScale / anOldWindowSize.x) * aNewWindowSize.x;

		if (myIsFullscreen == false)
		{
			CU::Vector2<float> unitRatioSize = myGruntUnit->GetSize() / anOldWindowSize.x;
			CU::Vector2<float> portraitRatioSize = myBuildingPortrait->GetSize() / anOldWindowSize.x;

			myGruntUnit->SetSize(unitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myRangerUnit->SetSize(unitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myTankUnit->SetSize(unitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myGruntPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myRangerPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myTankPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myBuildingPortrait->SetSize(portraitRatioSize * aNewWindowSize.x, { 0.f, 0.f });
			myUnitPosition = (myUnitPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myPortraitPosition = (myPortraitPosition / anOldWindowSize.x) * aNewWindowSize.x;
			myStatsSprite->SetSize((myStatsSprite->GetSize() / anOldWindowSize.x) * aNewWindowSize.x, { 0.f, 0.f });
		}
		else
		{
			CU::Vector2<float> unitRatioSize = myGruntUnit->GetSize() / anOldWindowSize;
			CU::Vector2<float> portraitRatioSize = myBuildingPortrait->GetSize() / anOldWindowSize;

			myGruntUnit->SetSize(unitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myRangerUnit->SetSize(unitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myTankUnit->SetSize(unitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myGruntPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myRangerPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myTankPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myBuildingPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
			myUnitPosition = (myUnitPosition / anOldWindowSize) * aNewWindowSize;
			myPortraitPosition = (myPortraitPosition / anOldWindowSize) * aNewWindowSize;
			myStatsSprite->SetSize((myStatsSprite->GetSize() / anOldWindowSize) * aNewWindowSize, { 0.f, 0.f });
		}
	}

	void UnitInfoWidget::RenderBaseInfo(const CU::Vector2<float>& aParentPosition)
	{
		myBuildingPortrait->Render(myPosition + aParentPosition + myPortraitPosition);

		if (myBuilding.GetEntityToSpawn() != eEntityType::EMPTY)
		{
			myBuildingTimer->Update(0.f);
			CU::Vector2<float> position = myPosition + aParentPosition + myUnitPosition + (myBuildingTimer->GetSize() / 2.f);
			myBuildingTimer->Render(position);
			position.x += myBuildingTimer->GetSize().x + 10.f;
			Prism::Engine::GetInstance()->PrintText(myBuilding.GetSpawnQueueSize(), position, Prism::eTextType::RELEASE_TEXT, myTextScale);
		}
	}

	void UnitInfoWidget::RenderUnitInfo(const CU::Vector2<float>& aParentPosition)
	{
		if (myUnits[0]->GetType() != eEntityType::UNIT) return;
		CU::Vector2<float> portraitPosition = myPosition + aParentPosition + myPortraitPosition;
		CU::Vector2<float> upgradePosition = portraitPosition;

		CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);
		HealthComponent* toCheck = myUnits[0]->GetComponent<HealthComponent>();
		if (toCheck->GetIsHealing() == true && toCheck->GetCurrentHealth() < toCheck->GetMaxHealth())
		{
			color = { 0.f, 1.f, 0.f, 1.f };
		}
		else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.3f)
		{
			color = { 1.f, 0.f, 0.f, 1.f };
		}
		else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.5f)
		{
			color = { 1.f, 0.4f, 0.4f, 1.f };
		}
		else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.7f)
		{
			color = { 1.f, 0.7f, 0.7f, 1.f };
		}

		if (myUnits[0]->GetUnitType() == eUnitType::GRUNT)
		{
			myGruntPortrait->Render(portraitPosition, { 1.f, 1.f }, color);
		}
		else if (myUnits[0]->GetUnitType() == eUnitType::RANGER)
		{
			myRangerPortrait->Render(portraitPosition, { 1.f, 1.f }, color);
		}
		else
		{
			myTankPortrait->Render(portraitPosition, { 1.f, 1.f }, color);
		}

		upgradePosition.x += myGruntPortrait->GetSize().y * 0.9f;

		myUnits[0]->GetComponent<PromotionComponent>()->RenderPromotion(portraitPosition);

		Prism::Engine::GetInstance()->PrintText(myBuilding.GetUpgradeLevel(myUnits[0]->GetUnitType()), upgradePosition, Prism::eTextType::RELEASE_TEXT);

		portraitPosition.y -= myGruntPortrait->GetSize().y / 3.5f;
		portraitPosition.x += myGruntPortrait->GetSize().x / 3.f;

		std::string currentHealth = std::to_string(int(myUnits[0]->GetComponent<HealthComponent>()->GetCurrentHealth()));
		Prism::Engine::GetInstance()->PrintText(currentHealth, portraitPosition, Prism::eTextType::RELEASE_TEXT, myTextScale, color);

		std::string maxHealth = "/" + std::to_string(int(myUnits[0]->GetComponent<HealthComponent>()->GetMaxHealth()));
		Prism::Engine::GetInstance()->PrintText(maxHealth, { portraitPosition.x + 25.f, portraitPosition.y }
		, Prism::eTextType::RELEASE_TEXT, myTextScale);

		color.x = 1.f;
		color.y = 1.f;
		color.z = 1.f;
		color.w = 1.f;

		if (myUnits[0]->GetUnitType() == eUnitType::TANK)
		{
			if (myUnits[0]->GetComponent<EnrageComponent>()->IsActive() == true)
			{
				color.x = 0.f;
				color.z = 0.f;
			}
		}

		CU::Vector2<float> position = { myGruntUnit->GetSize().x / 2.f, myPosition.y };
		position += aParentPosition + myUnitPosition;
		myStatsSprite->Render(position);

		position.x += myStatsSprite->GetSize().x / 20.f;
		position.y -= myStatsSprite->GetSize().y / 2.f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetComponent<HealthComponent>()->GetArmor()
			, position, Prism::eTextType::RELEASE_TEXT, myTextScale, color);

		position.x += myStatsSprite->GetSize().x / 3.f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetComponent<ActorComponent>()->GetAttackDamage()
			, position, Prism::eTextType::RELEASE_TEXT, myTextScale, color);

		position.x += myStatsSprite->GetSize().x / 3.f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetMaxSpeed()
			, position, Prism::eTextType::RELEASE_TEXT, myTextScale, color);
	}

	void UnitInfoWidget::RenderUnitGroupInfo(const CU::Vector2<float>& aParentPosition)
	{
		for (int i = 0; i < myUnits.Size(); i++)
		{
			Prism::SpriteProxy* portrait = nullptr;

			if (myUnits[i]->GetUnitType() == eUnitType::GRUNT)
			{
				portrait = myGruntUnit;
			}
			else if (myUnits[i]->GetUnitType() == eUnitType::RANGER)
			{
				portrait = myRangerUnit;
			}
			else
			{
				portrait = myTankUnit;
			}

			CU::Vector4<float> color(1.f, 1.f, 1.f, 1.f);
			HealthComponent* toCheck = myUnits[i]->GetComponent<HealthComponent>();
			if (toCheck->GetIsHealing() == true && toCheck->GetCurrentHealth() < toCheck->GetMaxHealth())
			{
				color = { 0.f, 1.f, 0.f, 1.f };
			}
			else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.3f)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}
			else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.5f)
			{
				color = { 1.f, 0.4f, 0.4f, 1.f };
			}
			else if (toCheck->GetCurrentHealth() < toCheck->GetMaxHealth() * 0.7f)
			{
				color = { 1.f, 0.7f, 0.7f, 1.f };
			}

			CU::Vector2<float> position = { (myPosition.x + portrait->GetSize().x * i) + (i * 10.f), myPosition.y };
			position += aParentPosition + myUnitPosition;

			CU::Vector2<float> upgradePosition = position;
			upgradePosition.x += portrait->GetSize().x * 0.7f;
			upgradePosition.y += portrait->GetSize().y * 0.15f;

			portrait->Render(position, { 1.f, 1.f }, color);
			position += portrait->GetSize() * 0.2f;
			myUnits[i]->GetComponent<PromotionComponent>()->RenderPromotion(position, { 0.7f, 0.7f });
			Prism::Engine::GetInstance()->PrintText(myBuilding.GetUpgradeLevel(myUnits[i]->GetUnitType()), upgradePosition, Prism::eTextType::RELEASE_TEXT, 0.8f);
		}
	}
}