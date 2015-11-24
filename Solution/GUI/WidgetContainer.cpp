#include "stdafx.h"
#include "WidgetContainer.h"
#include <Sprite.h>

namespace GUI
{
	WidgetContainer::WidgetContainer()
		: myBackground(nullptr)
	{
	}

	WidgetContainer::~WidgetContainer()
	{
		for (auto widget : myWidgets)
		{
			delete widget.second;
			widget.second = nullptr;
		}

		delete myBackground;
		myBackground = nullptr;
	}

	void WidgetContainer::AddWidget(const std::string& aName, Widget* aWidget)
	{
		std::string message = "[GUI] Widget with name " + aName + " already exists.";
		assert(myWidgets.find(aName) == myWidgets.end() && message.c_str());
		myWidgets[aName] = aWidget;
	}

	void WidgetContainer::Update()
	{
		for (auto widget : myWidgets)
		{
			widget.second->Update();
		}
	}

	void WidgetContainer::Render(const CU::Vector2<float>&)
	{
		for (auto widget : myWidgets)
		{
			widget.second->Render(myPosition);
		}
	}

	Widget* WidgetContainer::FindWidget(const std::string& aName)
	{
		auto widget = myWidgets.find(aName);
		if (widget != myWidgets.end())
		{
			return widget->second;
		}

		return nullptr;
	}

	Widget* WidgetContainer::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			for (auto widget : myWidgets)
			{
				if (widget.second->IsVisible() == true && widget.second->IsInside(aPosition - myPosition) == true)
				{
					Widget* childWidget = widget.second->MouseIsOver(aPosition - myPosition);
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