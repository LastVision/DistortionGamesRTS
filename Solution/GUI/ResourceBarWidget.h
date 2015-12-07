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

	class ResourceBarWidget : public Widget
	{

	public:
		ResourceBarWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int& someGold);
		~ResourceBarWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

	private:
		void operator=(ResourceBarWidget&) = delete;

		const int& myPlayerGold;

		Prism::Sprite* myGoldSprite;

	};
}