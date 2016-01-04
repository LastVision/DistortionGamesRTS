#pragma once
#include "Widget.h"
#include <StaticArray.h>
#include "../Game/GameDefines.h"

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;
class Entity;
class PlayerDirector;

namespace GUI
{
	class ControlGroupWidget : public Widget
	{

	public:
		ControlGroupWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer);
		~ControlGroupWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

	private:
		void operator=(ControlGroupWidget&) = delete;

		const CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS>& myControlGroups;
	};
}