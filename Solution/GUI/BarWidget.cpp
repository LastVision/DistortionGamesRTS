#include "stdafx.h"
#include "BarWidget.h"

namespace GUI
{
	BarWidget::BarWidget(const int& aMaxValue, const int& aCurrentValue, CU::Vector2<float> aSize)
		: myMaxValueInt(&aMaxValue)
		, myCurrentValueInt(&aCurrentValue)
		, myIsFloat(false)
	{
		mySize = aSize;

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_background.dds", mySize, mySize / 2.f);

		myValueSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_value.dds", mySize, mySize / 2.f);

		/*myBackgroundSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_background.dds", mySize, mySize / 2.f);
		myValueSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_value.dds", mySize, mySize / 2.f);*/
	}

	BarWidget::BarWidget(const float& aMaxValue, const float& aCurrentValue, CU::Vector2<float> aSize)
		: myMaxValueFloat(&aMaxValue)
		, myCurrentValueFloat(&aCurrentValue)
		, myIsFloat(true)
	{
		mySize = aSize;

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_background.dds", mySize, mySize / 2.f);

		myValueSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/T_healthbar_value.dds", mySize, mySize / 2.f);

		/*myBackgroundSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_background.dds", mySize, mySize / 2.f);
		myValueSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_healthbar_value.dds", mySize, mySize / 2.f);*/
	}

	BarWidget::~BarWidget()
	{
		SAFE_DELETE(myBackgroundSprite);
		SAFE_DELETE(myValueSprite);
	}

	void BarWidget::Update(float)
	{
		if (myIsFloat == false)
		{
			if (*myCurrentValueInt * mySize.x != myValueSprite->GetSize().x)
			{
				float newSize = float(*myCurrentValueInt) / float(*myMaxValueInt);
				myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
			}
		}
		else
		{
			if (*myCurrentValueFloat * mySize.x != myValueSprite->GetSize().x)
			{
				float newSize = *myCurrentValueFloat / *myMaxValueFloat;
				myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
			}
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
		float newSize;

		if (myIsFloat == false)
		{
			newSize = float(*myCurrentValueInt) / float(*myMaxValueInt);
		}
		else
		{
			newSize = *myCurrentValueFloat / *myMaxValueFloat;
		}

		myBackgroundSprite->SetSize(mySize, mySize / 2.f);
		myValueSprite->SetSize({ mySize.x * newSize, mySize.y }, mySize / 2.f);
	}
}