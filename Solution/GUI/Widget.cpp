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

	bool Widget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return	aPosition.x >= myPosition.x &&
			aPosition.x >= myPosition.x &&
			aPosition.x <= myPosition.x + mySize.x &&
			aPosition.x <= myPosition.x + mySize.y;
	}

	void Widget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		CU::Vector2<float> ratioPostion = myPosition / anOldSize;
		myPosition = ratioPostion * aNewSize;

		CU::Vector2<float> ratioSize = mySize / anOldSize;
		mySize = ratioSize * aNewSize;
	}
}