#include "stdafx.h"
#include <Engine.h>
#include <sstream>
#include "UnitCapWidget.h"

namespace GUI
{

	UnitCapWidget::UnitCapWidget(const CU::Vector2<float>& aSize, const int& aUnitCap, const int& aUnitCount)
		: myUnitCap(aUnitCap)
		, myUnitCount(aUnitCount)
	{
		myPosition = { 0.f, 0.f };
		mySize = aSize;
	}

	UnitCapWidget::~UnitCapWidget()
	{
	}

	void UnitCapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		std::stringstream ss;

		ss << myUnitCount << "/" << myUnitCap;

		Prism::Engine::GetInstance()->PrintText(ss.str(), myPosition + aParentPosition, Prism::eTextType::RELEASE_TEXT);
	}
}