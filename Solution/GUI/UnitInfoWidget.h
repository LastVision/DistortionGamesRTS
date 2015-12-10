#pragma once
#include "Widget.h"
#include "../Entity/EntityEnum.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace Prism
{
	class Sprite;
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
		void operator=(UnitInfoWidget&) = delete;

		const CU::GrowingArray<Entity*>& myUnits;
		const BuildingComponent& myBuilding;
		Prism::Sprite* myGruntPortrait;
		Prism::Sprite* myBuildingPortrait;
		Prism::Sprite* myGruntUnit;
		BarWidget* myBuildingTimer;

		eEntityType mySelectedType;
		bool myIsUnitSelected;

		CU::Vector2<float> myUnitPosition;
		CU::Vector2<float> myPortraitPosition;
	};
}