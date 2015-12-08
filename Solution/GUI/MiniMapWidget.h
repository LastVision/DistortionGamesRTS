#pragma once
#include "Widget.h"

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
	class MiniMapWidget : public Widget
	{

	public:
		MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~MiniMapWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		Prism::Sprite* myPlaceholderSprite;

	};
}