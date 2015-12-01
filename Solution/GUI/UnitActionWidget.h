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


namespace GUI
{
	class WidgetContainer;

	class UnitActionWidget : public Widget
	{
	public:
		UnitActionWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits);
		~UnitActionWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:

		const CU::GrowingArray<Entity*>& myUnits;

		WidgetContainer* myUnitActionButtons;
		WidgetContainer* myBuildingActionButtons;
	};
}
