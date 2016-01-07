#pragma once
#include "Widget.h"

namespace GUI
{
	class UnitCapWidget : public Widget
	{

	public:
		UnitCapWidget(const CU::Vector2<float>& aSize, const int& aUnitCap, const int& aUnitCount);
		~UnitCapWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

	private:
		void operator=(UnitCapWidget&) = delete;

		const int& myUnitCap;
		const int& myUnitCount;
	};
}