#pragma once
#include "Widget.h"

namespace Prism
{

	class Sprite;
}

namespace GUI
{
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget();
		~ButtonWidget();


	private:

		Prism::Sprite* myImageNormal;
		Prism::Sprite* myImagePressed;
		Prism::Sprite* myImageHover;
		Prism::Sprite* myCurrentImage;
	};
}