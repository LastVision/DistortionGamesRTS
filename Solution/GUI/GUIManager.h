#pragma once

namespace CU
{
	class InputWrapper;
}

class Entity;
class PlayerDirector;

namespace GUI
{
	class WidgetContainer;
	class Widget;
	class Cursor;

	class GUIManager
	{
	public:
		GUIManager(Cursor* aCursor, const std::string& aXMLPath, const PlayerDirector* aPlayer);
		~GUIManager();

		void AddWidget(Widget* aWidget);
		void Update();
		void Render();

		void OnResize(int aWidth, int aHeight);

		CU::Vector3<float> CalcCameraMovement() const;

		bool MouseOverGUI();

		void ReadXML(const std::string& aXMLPath);

	private:

		void CheckMousePressed();
		void CheckMouseDown();
		void CheckMouseReleased();
		void CheckMouseMoved();
		void CheckMouseEntered();
		void CheckMouseExited();

		const PlayerDirector* myPlayer;
		WidgetContainer* myWidgets;
		Widget* myActiveWidget;
		Cursor* myCursor;

		CU::Vector2<float> myMousePosition;
		CU::Vector2<float> myWindowSize;
	};
}