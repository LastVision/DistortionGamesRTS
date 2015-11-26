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
struct OnClickMessage;

namespace GUI
{
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~ButtonWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnMouseDown(const CU::Vector2<float>& aPosition) override;
		void OnMouseUp() override;
		void OnMouseEnter() override;
		void OnMouseExit() override;

		bool IsInside(const CU::Vector2<float>& aPosition) const override;
		void SetPosition(const CU::Vector2<float>& aPosition) override;

	private:
		void Click();
		
		Prism::Sprite* myImageNormal;
		Prism::Sprite* myImagePressed;
		Prism::Sprite* myImageHover;
		Prism::Sprite* myImageCurrent;

		OnClickMessage* myClickEvent;
	};
}