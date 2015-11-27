#pragma once
#include "Widget.h"
#include <GrowingArray.h>

namespace Prism
{
	class Sprite;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class WidgetContainer : public Widget
	{
	public:

		WidgetContainer(Prism::Sprite* aBackgroundSprite, bool aIsWindowSize = false);
		WidgetContainer(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~WidgetContainer();

		void AddWidget(Widget* aWidget);
		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

	private:
		Prism::Sprite* myBackground;
		CU::GrowingArray<Widget*> myWidgets;
		bool myIsWindowSize;
	};
}