#include "stdafx.h"
#include <Engine.h>
#include "WidgetContainer.h"

namespace GUI
{
	WidgetContainer::WidgetContainer(Prism::SpriteProxy* aBackgroundSprite, const CU::Vector2<float>& aSize)
		: myBackground(aBackgroundSprite)
		, myWidgets(8)
	{
		mySize = aSize;
	}

	WidgetContainer::~WidgetContainer()
	{
		myWidgets.DeleteAll();
		SAFE_DELETE(myBackground);
	}

	void WidgetContainer::AddWidget(Widget* aWidget)
	{
		myWidgets.Add(aWidget);
		aWidget->SetParent(this);
	}

	void WidgetContainer::Update(float aDelta)
	{
		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->Update(aDelta);
		}
	}

	void WidgetContainer::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsVisible == true)
		{
			if (myBackground != nullptr)
			{
				myBackground->Render(myPosition);
			}

			for (int i = 0; i < myWidgets.Size(); i++)
			{
				myWidgets[i]->Render(myPosition + aParentPosition);
			}
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

	void WidgetContainer::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);

		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->OnResize(aNewSize, anOldSize);
		}
		if (myBackground != nullptr)
		{
			CU::Vector2<float> ratio = myBackground->GetSize() / anOldSize;
			myBackground->SetSize(aNewSize * ratio, { 0.f, 0.f });
		}
	}
}