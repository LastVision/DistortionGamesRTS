#pragma once
#include "Widget.h"
#include <Sprite.h>

namespace GUI
{
	class BarWidget : public Widget
	{
	public:
		BarWidget(const int& aMaxValue, const int& aCurrentValue, CU::Vector2<float> aSize);
		~BarWidget();

		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		void operator=(BarWidget&) = delete;

		Prism::Sprite* myBackgroundSprite;
		Prism::Sprite* myValueSprite;

		const int& myMaxValue;
		const int& myCurrentValue;
	};			   
}