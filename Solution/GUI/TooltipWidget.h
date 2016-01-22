#pragma once
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class GUIManager;

	class TooltipWidget : public Widget
	{
	public:
		TooltipWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const GUIManager* aGuiManager);
		~TooltipWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget*	MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen) override;

	private:

		CU::Vector2<float> myTextPosition; 

		const Widget* myActiveWidget;
		const GUIManager* myGuiManager;

		Prism::SpriteProxy* myBackground;

	};
}
