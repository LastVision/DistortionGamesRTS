#pragma once
#include "Widget.h"
#include <StaticArray.h>
#include "../Game/GameDefines.h"

class Entity;

namespace GUI
{
	class ControlGroupWidget : public Widget
	{

	public:
		ControlGroupWidget(const CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS>& someControlGroups);
		~ControlGroupWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

	private:

		const CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS>& myControlGroups;
	};
}