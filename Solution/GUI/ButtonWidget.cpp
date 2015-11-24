#include "stdafx.h"
#include "ButtonWidget.h"
#include <Sprite.h>

namespace GUI
{
	ButtonWidget::ButtonWidget()
		: myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
	{
	}

	ButtonWidget::~ButtonWidget()
	{
		delete myImageNormal;
		delete myImagePressed;
		delete myImageHover;
		myImageNormal = nullptr;
		myImagePressed = nullptr;
		myImageHover = nullptr;
		myImageCurrent = nullptr;
	}

	void ButtonWidget::Render(const CU::Vector2<float>&)
	{
		myImageCurrent->Render(myPosition);
	}

	void ButtonWidget::OnMousePressed(const CU::Vector2<float>&)
	{
		myImageCurrent = myImagePressed;
		Click();
	}

	void ButtonWidget::OnMouseDown(const CU::Vector2<float>&)
	{
		Click();
	}

	void ButtonWidget::OnMouseUp()
	{
		myImageCurrent = myImageNormal;
	}

	void ButtonWidget::OnMouseEnter()
	{
		myImageCurrent = myImageHover;
	}

	void ButtonWidget::OnMouseExit()
	{
		myImageCurrent = myImageNormal;
	}

	void ButtonWidget::Click()
	{

	}
}