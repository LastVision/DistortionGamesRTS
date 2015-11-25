#pragma once
#include "Widget.h"
#include <GrowingArray.h>

namespace Prism
{
	class Sprite;
}

namespace GUI
{
	class WidgetContainer : public Widget
	{
	public:

		WidgetContainer();
		~WidgetContainer();

		void AddWidget(Widget* aWidget);
		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

	private:
		Prism::Sprite* myBackground;
		CU::GrowingArray<Widget*> myWidgets;
	};
}