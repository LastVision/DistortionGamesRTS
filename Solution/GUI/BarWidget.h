#pragma once
#include "Widget.h"
#include <Sprite.h>

namespace GUI
{
	class BarWidget : public Widget
	{
	public:
		BarWidget(const int& aMaxValue, const int& aCurrentValue);
		~BarWidget();

		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

	private:
		void operator=(BarWidget&) = delete;

		Prism::Sprite* myBackgroundSprite;
		Prism::Sprite* myValueSprite;

		const int& myMaxValue;
		const int& myCurrentValue;
	};			   
}