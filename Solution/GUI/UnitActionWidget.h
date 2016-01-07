#pragma once
#include "Widget.h"

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
class PlayerDirector;

namespace GUI
{
	class WidgetContainer;

	class UnitActionWidget : public Widget
	{
	public:
		UnitActionWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits 
			, const PlayerDirector* aPlayer);
		~UnitActionWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;
		void Update() override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:

		WidgetContainer* ReadContainer(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, CU::Vector2<float> aSize);

		void operator=(UnitActionWidget&) = delete;

		const CU::GrowingArray<Entity*>& myUnits;

		const PlayerDirector* myPlayer;

		WidgetContainer* myUnitActionButtons;
		WidgetContainer* myBuildingActionButtons;

		WidgetContainer* myGruntActionButtons;
		WidgetContainer* myRangerActionButtons;
		WidgetContainer* myTankActionButtons;

		bool myIsUnitSelected;
		eEntityType mySelectedType;

		bool myHasSelectedGrunt;
		bool myHasSelectedRanger;
		bool myHasSelectedTank;
	};
}
