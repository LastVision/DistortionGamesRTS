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
		GUIManager(CU::InputWrapper* anInputWrapper, Cursor* aCursor);
		~GUIManager();

		void AddWidget(const std::string& aName, Widget* aWidget);
		void Update();
		void Render();

		void SetPosition(const CU::Vector2<float>& aPosition);
		void SetSize(const CU::Vector2<float>& aSize);

		Widget* FindWidget(const std::string& aWidgetName);

	private:

		void CheckMousePressed();
		void CheckMouseDown();
		void CheckMouseReleased();
		void CheckMouseMoved();
		void CheckMouseEntered();
		void CheckMouseExited();

		Widget* myActiveWidget;
		WidgetContainer* myWidgets;
		CU::InputWrapper* myInputWrapper;
		Cursor* myCursor;

		CU::Vector2<float> myMousePosition;
	};
}