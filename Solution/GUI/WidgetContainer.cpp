#include "stdafx.h"
#include <Engine.h>
#include "WidgetContainer.h"

namespace GUI
{
	WidgetContainer::WidgetContainer(Prism::SpriteProxy* aBackgroundSprite, const CU::Vector2<float>& aSize, bool aIsFullscreen)
		: Widget()
		, myBackground(aBackgroundSprite)
		, myWidgets(8)
	{
		myIsFullscreen = aIsFullscreen;
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

	void WidgetContainer::Render(const CU::Vector2<float>& aParentPosition, int anIndex)
	{
		DL_ASSERT_EXP(myWidgets.Size() >= anIndex, "[WidgetContainer] Trying to render nonexisting index.");

		if (myBackground != nullptr)
		{
			myBackground->Render(myPosition);
		}

		myWidgets[anIndex]->Render(myPosition + aParentPosition);
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

	Widget* WidgetContainer::MouseIsOver(const CU::Vector2<float>& aPosition, int anIndex)
	{
		if (IsInside(aPosition) == true)
		{
			if (myWidgets[anIndex]->IsVisible() == true && myWidgets[anIndex]->IsInside(aPosition - myPosition) == true)
			{
				Widget* childWidget = myWidgets[anIndex]->MouseIsOver(aPosition - myPosition);
				if (childWidget != nullptr)
				{
					return childWidget;
				}
			}

			return this;
		}
		return nullptr;
	}

	void WidgetContainer::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewSize, anOldSize, aIsFullScreen);

		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->OnResize(aNewSize, anOldSize, false);
		}
		if (myBackground != nullptr)
		{
			if (myIsFullscreen == false)
			{
				CU::Vector2<float> ratio = myBackground->GetSize() / anOldSize.x;
				myBackground->SetSize(aNewSize.x * ratio, { 0.f, 0.f });
			}
			else
			{
				CU::Vector2<float> ratio = myBackground->GetSize() / anOldSize;
				myBackground->SetSize(aNewSize * ratio, { 0.f, 0.f });
			}
		}
	}
}