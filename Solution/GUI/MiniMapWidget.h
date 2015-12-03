#pragma once
#include "Widget.h"

namespace GUI
{
	class MiniMapWidget : public Widget
	{

	public:
		MiniMapWidget();
		~MiniMapWidget();

		void OnMouseDown(const CU::Vector2<float>& aPosition) override;

	private:


	};
}