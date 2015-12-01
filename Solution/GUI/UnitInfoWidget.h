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

	class UnitInfoWidget : public Widget
	{

	public:
		UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits);
		~UnitInfoWidget();

		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		Prism::Sprite* myUnitPortrait;
		Prism::Sprite* myBackground;

		const CU::GrowingArray<Entity*>& myUnits;
	};
}