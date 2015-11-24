#pragma once
#include "Widget.h"
#include <unordered_map>

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

		void AddWidget(const std::string& aName, Widget* aWidget);
		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* FindWidget(const std::string& aName) override;
		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

	private:
		Prism::Sprite* myBackground;
		std::unordered_map<std::string, Widget*> myWidgets;
	};
}