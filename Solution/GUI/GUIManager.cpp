#include "stdafx.h"
#include "Cursor.h"
#include <Engine.h>
#include "GUIManager.h"
#include "../InputWrapper/InputWrapper.h"
#include "WidgetContainer.h"
#include "ButtonWidget.h"
#include <Sprite.h>

namespace GUI
{
	GUIManager::GUIManager(Cursor* aCursor)
		: myActiveWidget(nullptr)
		, myCursor(aCursor)
		, myMousePosition({ 0.f, 0.f })
	{
		myWidgets = new WidgetContainer;

		CU::Vector2<float> buttonSize(100.f, 100.f);

		ButtonWidget* widget = new ButtonWidget;
		widget->myImageHover = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start_hover.dds", buttonSize, buttonSize / 2.f);
		widget->myImageNormal = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start.dds", buttonSize, buttonSize / 2.f);
		widget->myImagePressed = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start_pressed.dds", buttonSize, buttonSize / 2.f);
		widget->myImageCurrent = widget->myImageNormal;
		widget->SetPosition({ 0.f, 0.f});
		widget->SetSize(buttonSize);
		myWidgets->AddWidget(widget);

		ButtonWidget* widget2 = new ButtonWidget;
		widget2->myImageHover = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start_hover.dds", buttonSize, buttonSize / 2.f);
		widget2->myImageNormal = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start.dds", buttonSize, buttonSize / 2.f);
		widget2->myImagePressed = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start_pressed.dds", buttonSize, buttonSize / 2.f);
		widget2->myImageCurrent = widget2->myImageNormal;
		widget2->SetPosition({ 200.f, 200.f });
		widget2->SetSize(buttonSize);
		myWidgets->AddWidget(widget2);

		SetPosition({ 0.f, 0.f });
		SetSize({ float(Prism::Engine::GetInstance()->GetWindowSize().x), float(Prism::Engine::GetInstance()->GetWindowSize().y) });
	}

	GUIManager::~GUIManager()
	{
		delete myWidgets;
		myWidgets = nullptr;
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
	}

	void GUIManager::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myWidgets->SetPosition(aPosition);
	}
	
	void GUIManager::SetSize(const CU::Vector2<float>& aSize)
	{
		myWidgets->SetSize(aSize);
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