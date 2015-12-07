#include "stdafx.h"
#include "BarWidget.h"

namespace GUI
{
	BarWidget::BarWidget(const int& aMaxValue, const int& aCurrentValue)
		: myMaxValue(aMaxValue)
		, myCurrentValue(aCurrentValue)
	{
		myBackgroundSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_background.dds", { aMaxValue * 10.f, 20.f });
		myValueSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_value.dds", { aMaxValue * 10.f, 20.f });
	}

	BarWidget::~BarWidget()
	{
		SAFE_DELETE(myBackgroundSprite);
		SAFE_DELETE(myValueSprite);
	}

	void BarWidget::Update()
	{
		if (myCurrentValue * 10.f != myValueSprite->GetSize().x)
		{
			float newSize = float(myCurrentValue) / float(myMaxValue);
			newSize *= myMaxValue;
			myValueSprite->SetSize({ newSize * 10.f, myValueSprite->GetSize().y }, { 0.f, 0.f });
		}
	}

	void BarWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myBackgroundSprite->Render(myPosition + aParentPosition);
		myValueSprite->Render(myPosition + aParentPosition);
	}
}