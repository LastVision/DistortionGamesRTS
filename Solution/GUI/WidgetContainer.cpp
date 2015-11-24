#include "stdafx.h"
#include "WidgetContainer.h"
#include <Sprite.h>

namespace GUI
{
	WidgetContainer::WidgetContainer()
		: myBackground(nullptr)
		, myWidgets(8)
	{
	}

	WidgetContainer::~WidgetContainer()
	{
		myWidgets.DeleteAll();

		delete myBackground;
		myBackground = nullptr;
	}

	void WidgetContainer::AddWidget(Widget* aWidget)
	{
		myWidgets.Add(aWidget);
	}

	void WidgetContainer::Update()
	{
		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->Update();
		}
	}

	void WidgetContainer::Render(const CU::Vector2<float>& aParentPosition)
	{
		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->Render(myPosition);
		}
	}

	Widget* WidgetContainer::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			for (int i = 0; i < myWidgets.Size(); i++)
			{
				if (myWidgets[i]->IsVisible() == true && myWidgets[i]->IsInside(aPosition - myPosition) == true)
				{
					Widget* childWidget = myWidgets[i]->MouseIsOver(aPosition - myPosition);
					if (childWidget != nullptr)
					{
						return childWidget;
					}
				}
			}

			return this;
		}
		return nullptr;
	}
}