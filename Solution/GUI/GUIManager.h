#pragma once

namespace CU
{
	class InputWrapper;
}

namespace GUI
{
	class WidgetContainer;
	class Widget;
	class Cursor;

	class GUIManager
	{
	public:
		GUIManager(Cursor* aCursor, const std::string& aXMLPath);
		~GUIManager();

		void AddWidget(Widget* aWidget);
		void Update();
		void Render();

		void SetPosition(const CU::Vector2<float>& aPosition);
		void SetSize(const CU::Vector2<float>& aSize);

	private:

		void CheckMousePressed();
		void CheckMouseDown();
		void CheckMouseReleased();
		void CheckMouseMoved();
		void CheckMouseEntered();
		void CheckMouseExited();

		Widget* myActiveWidget;
		WidgetContainer* myWidgets;
		Cursor* myCursor;

		CU::Vector2<float> myMousePosition;
	};
}