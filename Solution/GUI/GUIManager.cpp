#include "stdafx.h"
#include "ButtonWidget.h"
#include "Cursor.h"
#include <Engine.h>
#include "../Game/PlayerDirector.h"
#include "GUIManager.h"
#include "../InputWrapper/InputWrapper.h"
#include "MiniMapWidget.h"
#include "ResourceBarWidget.h"
#include <Sprite.h>
#include "UnitActionWidget.h"
#include "UnitInfoWidget.h"
#include "WidgetContainer.h"
#include <XMLReader.h>

namespace GUI
{
	GUIManager::GUIManager(Cursor* aCursor, const std::string& aXMLPath, const PlayerDirector* aPlayer)
		: myActiveWidget(nullptr)
		, myCursor(aCursor)
		, myPlayer(aPlayer)
		, myMousePosition({ 0.f, 0.f })
	{
		ReadXML(aXMLPath);
	}

	GUIManager::~GUIManager()
	{
		SAFE_DELETE(myWidgets);
		myActiveWidget = nullptr;
	}

	void GUIManager::AddWidget(Widget* aWidget)
	{
		myWidgets->AddWidget(aWidget);
	}

	void GUIManager::Update()
	{
		myMousePosition = myCursor->GetMousePosition();

		myWidgets->Update();

		CheckMouseMoved();
		CheckMouseExited();
		CheckMouseDown();
		CheckMousePressed();
		CheckMouseReleased();

		CheckMouseEntered();
	}

	void GUIManager::Render()
	{
		myWidgets->Render({ 0.f, 0.f });
	}

	void GUIManager::OnResize(int aWidth, int aHeight)
	{
		CU::Vector2<float> newSize = { float(aWidth), float(aHeight) };
		myWidgets->OnResize(newSize, myWindowSize);
		myWindowSize = newSize;
	}

	CU::Vector3<float> GUIManager::CalcCameraMovement() const
	{
		CU::Vector3<float> movement;

		float epsilon = 0.05f;
		if (myCursor->GetMousePositionZeroToOne().x < epsilon)
		{
			movement.x = -1.f;
		}
		if (myCursor->GetMousePositionZeroToOne().x > 1.f - epsilon)
		{
			movement.x = 1.f;
		}

		if (myCursor->GetMousePositionZeroToOne().y < epsilon)
		{
			movement.z = -1.f;
		}
		if (myCursor->GetMousePositionZeroToOne().y > 1.f - epsilon)
		{
			movement.z = 1.f;
		}

		return movement;
	}

	bool GUIManager::MouseOverGUI()
	{
		return myActiveWidget != nullptr && myActiveWidget != myWidgets;
	}

	void GUIManager::ReadXML(const std::string& aXMLPath)
	{
		myWindowSize = { 1920.f, 1080.f }; // XML coordinates respond to this resolution, will be resized

		std::string path = "";
		CU::Vector2<float> size;
		CU::Vector2<float> position;

		XMLReader reader;
		reader.OpenDocument(aXMLPath);

		tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");

		myWidgets = new WidgetContainer(nullptr, myWindowSize);
		myWidgets->SetPosition({ 0.f, 0.f });

		tinyxml2::XMLElement* containerElement = reader.ForceFindFirstChild(rootElement, "container");
		for (; containerElement != nullptr; containerElement = reader.FindNextElement(containerElement))
		{
			Prism::Sprite* backgroundSprite = nullptr;

			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "size"), "x", size.x);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "size"), "y", size.y);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "position"), "x", position.x);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "position"), "y", position.y);

			tinyxml2::XMLElement* spriteElement = reader.FindFirstChild(containerElement, "backgroundsprite");
			tinyxml2::XMLElement* spriteSizeElement = reader.FindFirstChild(containerElement, "backgroundsize");

			if (spriteElement != nullptr)
			{
				reader.ForceReadAttribute(spriteElement, "path", path);

				if (spriteSizeElement != nullptr)
				{
					CU::Vector2<float> spriteSize;
					reader.ForceReadAttribute(spriteSizeElement, "x", spriteSize.x);
					reader.ForceReadAttribute(spriteSizeElement, "y", spriteSize.y);
					backgroundSprite = new Prism::Sprite(path, spriteSize);
				}
				else
				{
					backgroundSprite = new Prism::Sprite(path, size);
				}
			}

			GUI::WidgetContainer* container = new WidgetContainer(backgroundSprite, size);
			container->SetPosition(position);

			tinyxml2::XMLElement* widgetElement = reader.FindFirstChild(containerElement, "widget");
			for (; widgetElement != nullptr; widgetElement = reader.FindNextElement(widgetElement))
			{
				std::string type = "";

				reader.ForceReadAttribute(widgetElement, "type", type);

				if (type == "button")
				{
					ButtonWidget* button = new ButtonWidget(&reader, widgetElement);
					container->AddWidget(button);
				}
				else if (type == "unit_info")
				{
					UnitInfoWidget* unitInfo = new UnitInfoWidget(&reader, widgetElement, myPlayer);
					container->AddWidget(unitInfo);
				}
				else if (type == "unit_action")
				{
					UnitActionWidget* unitActions = new UnitActionWidget(&reader, widgetElement, myPlayer->GetSelectedUnits());
					container->AddWidget(unitActions);
				}
				else if (type == "minimap")
				{
					MiniMapWidget* minimap = new MiniMapWidget(&reader, widgetElement);
					container->AddWidget(minimap);
				}
				else if (type == "resourcebar")
				{
					ResourceBarWidget* resourceBar = new ResourceBarWidget(&reader, widgetElement, 0);
					container->AddWidget(resourceBar);
				}
			}
			myWidgets->AddWidget(container);
		}

		reader.CloseDocument();
	}

	void GUIManager::CheckMousePressed()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) == true)
			{
				myActiveWidget->OnMousePressed(myMousePosition);
			}
		}
	}
	
	void GUIManager::CheckMouseDown()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseDown(0) == true)
			{
				myActiveWidget->OnMouseDown(myMousePosition);
			}
		}
	}
	
	void GUIManager::CheckMouseReleased()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseUp(0) == true)
			{
				myActiveWidget->OnMouseUp();
				myActiveWidget->OnMouseEnter(); // hover button again after pressing it
			}
		}
	}
	
	void GUIManager::CheckMouseMoved()
	{
		if (myActiveWidget != nullptr)
		{
			if (myMousePosition.x != 0 || myMousePosition.y != 0)
			{
				myActiveWidget->OnMouseMoved(myMousePosition);
			}
		}
	}
	
	void GUIManager::CheckMouseEntered()
	{
		Widget* activeWidget = myWidgets->MouseIsOver(myMousePosition);

		if (activeWidget != nullptr && activeWidget != myActiveWidget)
		{
			activeWidget->OnMouseEnter();
			myActiveWidget = activeWidget;
		}
	}
	
	void GUIManager::CheckMouseExited()
	{
		Widget* activeWidget = myWidgets->MouseIsOver(myMousePosition);

		if (myActiveWidget != activeWidget)
		{
			if (myActiveWidget != nullptr)
			{
				myActiveWidget->OnMouseExit();
				myActiveWidget = nullptr;
			}
		}
	}
}