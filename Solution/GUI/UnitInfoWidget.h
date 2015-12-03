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

namespace GUI
{
	class UnitInfoWidget : public Widget
	{

	public:
		UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits);
		~UnitInfoWidget();

		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		void operator=(UnitInfoWidget&) = delete;

		const CU::GrowingArray<Entity*>& myUnits;
		Prism::Sprite* myUnitPortrait;
		Prism::Sprite* myBuildingPortrait;

		bool myIsUnitSelected;
		eEntityType mySelectedType;
	};
}