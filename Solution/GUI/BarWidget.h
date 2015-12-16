#pragma once
#include "Widget.h"

namespace Prism
{
	class Sprite;
}

namespace GUI
{
	class BarWidget : public Widget
	{
	public:
		BarWidget(const int& aMaxValue, const int& aCurrentValue, CU::Vector2<float> aSize);
		BarWidget(const float& aMaxValue, const float& aCurrentValue, CU::Vector2<float> aSize);
		~BarWidget();

		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		void operator=(BarWidget&) = delete;

		Prism::Sprite* myBackgroundSprite;
		Prism::Sprite* myValueSprite;

		bool myIsFloat;
		
		union
		{
			const float* myCurrentValueFloat;
			const int* myCurrentValueInt;
		};

		union
		{
			const float* myMaxValueFloat;
			const int* myMaxValueInt;
		};
	};			   
}