#include "stdafx.h"
#include "ButtonWidget.h"
#include "Cursor.h"
#include <Engine.h>
#include "GUIManager.h"
#include "../InputWrapper/InputWrapper.h"
#include <Sprite.h>
#include "WidgetContainer.h"
#include <XMLReader.h>

namespace GUI
{
	GUIManager::GUIManager(Cursor* aCursor, const std::string& aXMLPath)
		: myActiveWidget(nullptr)
		, myCursor(aCursor)
		, myMousePosition({ 0.f, 0.f })
	{
		myWindowSize = { 1920.f, 1080.f }; // XML coordinates respond to this resolution, will be reseized

		std::string path = "";
		CU::Vector2<float> size;

		XMLReader reader;
		reader.OpenDocument(aXMLPath);

		tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "backgroundsprite"), "path", path);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "backgroundsprite"), "sizex", size.x);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "backgroundsprite"), "sizey", size.y);

		Prism::Sprite* backgroundSprite = new Prism::Sprite(path, size);
		myWidgets = new WidgetContainer(backgroundSprite);

		tinyxml2::XMLElement* widgetElement = reader.FindFirstChild(rootElement, "widget");
		for (; widgetElement != nullptr; widgetElement = reader.FindNextElement(widgetElement))
		{
			std::string type = "";

			reader.ForceReadAttribute(widgetElement, "type", type);

			if (type == "container")
			{
				WidgetContainer* container = new WidgetContainer(&reader, widgetElement);
				myWidgets->AddWidget(container);
			}
			else if (type == "button")
			{
				ButtonWidget* container = new ButtonWidget(&reader, widgetElement);
				myWidgets->AddWidget(container);
			}
		}

		reader.CloseDocument();

		SetSize(myWindowSize * 2.f);
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
			Prism::Engine::GetInstance()->PrintText(myActiveWidget->GetHoverText(), { 500.f, -500.f }, Prism::eTextType::RELEASE_TEXT);
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

	void GUIManager::OnResize(int aHeight, int aWidth)
	{
		CU::Vector2<float> newSize = { float(aHeight), float(aWidth) };
		myWidgets->OnResize(newSize, myWindowSize);
		myWindowSize = newSize;
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