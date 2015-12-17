#pragma once
#include "Widget.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace Prism
{
	class SpriteProxy;
}

class XMLReader;
class Entity;
class BuildingComponent;
class PlayerDirector;

namespace GUI
{
	class BarWidget;

	class UnitInfoWidget : public Widget
	{

	public:
		UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer);
		~UnitInfoWidget();

		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		void RenderBaseInfo(const CU::Vector2<float>& aParentPosition);
		void RenderUnitInfo(const CU::Vector2<float>& aParentPosition);

		void operator=(UnitInfoWidget&) = delete;

		const CU::GrowingArray<Entity*>& myUnits;
		const BuildingComponent& myBuilding;
		Prism::SpriteProxy* myGruntPortrait;
		Prism::SpriteProxy* myBuildingPortrait;
		Prism::SpriteProxy* myGruntUnit;
		Prism::SpriteProxy* myStatsSprite;
		BarWidget* myBuildingTimer;

		eEntityType mySelectedType;
		bool myIsUnitSelected;

		CU::Vector2<float> myUnitPosition;
		CU::Vector2<float> myPortraitPosition;
		float myTextScale;
	};
}