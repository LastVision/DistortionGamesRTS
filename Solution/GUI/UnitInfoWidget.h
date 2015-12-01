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
	class UnitInfoWidget : public Widget
	{

	public:
		UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits);
		~UnitInfoWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		void operator=(UnitInfoWidget&) = delete;

		Prism::Sprite* myUnitPortrait;

		const CU::GrowingArray<Entity*>& myUnits;
	};
}