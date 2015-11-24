#pragma once
#include "Widget.h"

namespace Prism
{

	class Sprite;
}

namespace GUI
{
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget();
		~ButtonWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnMouseDown(const CU::Vector2<float>& aPosition) override;
		void OnMouseUp() override;
		void OnMouseEnter() override;
		void OnMouseExit() override;

		bool IsInside(const CU::Vector2<float>& aPosition) const override;
		void SetPosition(const CU::Vector2<float>& aPosition) override;

		Prism::Sprite* myImageNormal;
		Prism::Sprite* myImagePressed;
		Prism::Sprite* myImageHover;
		Prism::Sprite* myImageCurrent;
	private:

		void Click();

	};
}