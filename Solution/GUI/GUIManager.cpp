#include "stdafx.h"
#include "ButtonWidget.h"
#include "Cursor.h"
#include <Engine.h>
#include "GUIManager.h"
#include "../InputWrapper/InputWrapper.h"
#include <Sprite.h>
#include "WidgetContainer.h"
#include <XMLReader.h>
#include "UnitInfoWidget.h"

namespace GUI
{
	GUIManager::GUIManager(Cursor* aCursor, const std::string& aXMLPath, const CU::GrowingArray<Entity*>& someUnits)
		: myActiveWidget(nullptr)
		, myCursor(aCursor)
		, myMousePosition({ 0.f, 0.f })
	{
		myWindowSize = { 1920.f, 1080.f }; // XML coordinates respond to this resolution, will be resized

		std::string path = "";
		CU::Vector2<float> size;

		XMLReader reader;
		reader.OpenDocument(aXMLPath);

		tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");

		myWidgets = new WidgetContainer(nullptr, myWindowSize);

		tinyxml2::XMLElement* containerElement = reader.ForceFindFirstChild(rootElement, "container");
		for (; containerElement != nullptr; containerElement = reader.FindNextElement(containerElement))
		{
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "backgroundsprite"), "path", path);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "backgroundsprite"), "sizex", size.x);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "backgroundsprite"), "sizey", size.y);
			Prism::Sprite* backgroundSprite = new Prism::Sprite(path, size);
			GUI::WidgetContainer* container = new WidgetContainer(backgroundSprite, size);

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
				else if (type == "unitinfo")
				{
					UnitInfoWidget* unitInfo = new UnitInfoWidget(&reader, widgetElement, someUnits);
					container->AddWidget(unitInfo);
				}
			}
			myWidgets->AddWidget(container);
		}

		reader.CloseDocument();
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

		CheckMouseMoved();
		CheckMouseExited();
		CheckMouseDown();
		CheckMousePressed();
		CheckMouseReleased();

		CheckMouseEntered();

		myWidgets->Update();
	}

	void GUIManager::Render()
	{
		myWidgets->Render({ 0.f, 0.f });

		if (myActiveWidget != nullptr && myActiveWidget->GetHoverText() != "")
		{
			CU::Vector2<float> hoverPosition = { myActiveWidget->GetPosition().x - myActiveWidget->GetSize().x / 2.f
				, myActiveWidget->GetPosition().y + myActiveWidget->GetSize().y / 2.f };
			
			Prism::Engine::GetInstance()->PrintText(myActiveWidget->GetHoverText(), hoverPosition
				, Prism::eTextType::RELEASE_TEXT);
		}
	}

	void GUIManager::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myWidgets->SetPosition(aPosition);
	}
	
	void GUIManager::SetSize(const CU::Vector2<float>& aSize)
	{
		myWidgets->SetSize(aSize);
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