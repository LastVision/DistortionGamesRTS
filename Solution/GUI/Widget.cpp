#include "stdafx.h"
#include "Widget.h"

namespace GUI
{
	Widget::Widget()
	{
		myIsVisible = true;
	}

	Widget::~Widget()
	{
	}

	void Widget::Destroy()
	{
	}

	void Widget::Update()
	{
	}

	void Widget::Render(const CU::Vector2<float>&)
	{
	}

	void Widget::OnMouseDown(const CU::Vector2<float>&)
	{
	}

	void Widget::OnMousePressed(const CU::Vector2<float>&)
	{
	}

	void Widget::OnMouseUp()
	{
	}

	void Widget::OnMouseEnter()
	{
	}

	void Widget::OnMouseExit()
	{
	}

	void Widget::OnMouseMoved(const CU::Vector2<float>&)
	{
	}

	Widget*	Widget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			return this;
		}
		else
		{
			return nullptr;
		}
	}

	Widget* Widget::FindWidget(const std::string&)
	{
		return nullptr;
	}

	bool Widget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return	aPosition.myX >= myPosition.myX &&
			aPosition.myY >= myPosition.myY &&
			aPosition.myX <= myPosition.myX + mySize.x &&
			aPosition.myY <= myPosition.myY + mySize.y;
	}
}