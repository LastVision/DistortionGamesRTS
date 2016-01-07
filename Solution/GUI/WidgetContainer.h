#pragma once
#include "Widget.h"
#include <GrowingArray.h>

namespace Prism
{
	class SpriteProxy;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class WidgetContainer : public Widget
	{
	public:

		WidgetContainer(Prism::SpriteProxy* aBackgroundSprite, const CU::Vector2<float>& aSize);
		~WidgetContainer();

		void AddWidget(Widget* aWidget);
		void Update(float aDelta) override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		int GetSize() const;

		Widget* GetFirstWidget();

	private:
		CU::GrowingArray<Widget*> myWidgets;
		Prism::SpriteProxy* myBackground;
	};

	inline int WidgetContainer::GetSize() const
	{
		return myWidgets.Size();
	}

	inline Widget* WidgetContainer::GetFirstWidget()
	{
		DL_ASSERT_EXP(myWidgets.Size() > 0, "Trying to get first widget from an empty container");
		return myWidgets[0];
	}
}