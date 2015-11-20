#include "stdafx.h"
#include "ButtonWidget.h"
#include <Sprite.h>

namespace GUI
{
	ButtonWidget::ButtonWidget()
		: myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myCurrentImage(nullptr)
	{
	}

	ButtonWidget::~ButtonWidget()
	{
		delete myImageNormal;
		delete myImagePressed;
		delete myImageHover;
		//delete myCurrentImage;
		myImageNormal = nullptr;
		myImagePressed = nullptr;
		myImageHover = nullptr;
		myCurrentImage = nullptr;
	}
}