#include "stdafx.h"
#include "ControlGroupWidget.h"

namespace GUI
{
	ControlGroupWidget::ControlGroupWidget(const CU::StaticArray<CU::GrowingArray<Entity*>, AMOUNT_OF_CONTROL_GROUPS>& someControlGroups)
		: myControlGroups(someControlGroups)
	{
	}

	ControlGroupWidget::~ControlGroupWidget()
	{
	}

	void ControlGroupWidget::Render(const CU::Vector2<float>& aParentPosition)
	{



	}
}