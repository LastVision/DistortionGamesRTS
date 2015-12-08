#include "stdafx.h"
#include "BarWidget.h"

namespace GUI
{
	BarWidget::BarWidget(const int& aMaxValue, const int& aCurrentValue, CU::Vector2<float> aSize)
		: myMaxValue(aMaxValue)
		, myCurrentValue(aCurrentValue)
	{
		mySize = aSize;
		myBackgroundSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_background.dds", mySize, mySize / 2.f);
		myValueSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_value.dds", mySize, mySize / 2.f);
	}

	BarWidget::~BarWidget()
	{
		SAFE_DELETE(myBackgroundSprite);
		SAFE_DELETE(myValueSprite);
	}

	void BarWidget::Update()
	{
		if (myCurrentValue * mySize.x != myValueSprite->GetSize().x)
		{
			float newSize = float(myCurrentValue) / float(myMaxValue);
			myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
		}
	}

	void BarWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myBackgroundSprite->Render(myPosition + aParentPosition);
		myValueSprite->Render(myPosition + aParentPosition);
	}

	void BarWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		float newSize = float(myCurrentValue) / float(myMaxValue);

		myBackgroundSprite->SetSize(mySize, mySize / 2.f);
		myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
	}
}