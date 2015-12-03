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

	private:
		Prism::Sprite* myPlaceholderSprite;

	};
}