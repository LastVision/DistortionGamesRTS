#include "stdafx.h"
#include <Engine.h>
#include "GUIManager.h"
#include "../InputWrapper/InputWrapper.h"
#include "WidgetContainer.h"
#include "ButtonWidget.h"
#include <Sprite.h>

namespace GUI
{
	GUIManager::GUIManager(CU::InputWrapper* anInputWrapper)
		: myActiveWidget(nullptr)
		, myInputWrapper(anInputWrapper)
		, myMousePosition({ 0.f, 0.f })
	{
		myWidgets = new WidgetContainer;

		ButtonWidget* widget = new ButtonWidget;
		widget->myImageHover = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start_hover.dds", { 100.f, 100.f }, { 50.f, 50.f });
		widget->myImageNormal = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start.dds", { 100.f, 100.f }, { 50.f, 50.f });
		widget->myImagePressed = new Prism::Sprite("Data/Resource/Texture/Menu/T_button_start.dds", { 100.f, 100.f }, { 50.f, 50.f });
		widget->myImageCurrent = widget->myImageNormal;
		widget->SetPosition({ 0.f, 0.f});
		myWidgets->AddWidget("test", widget);

		SetPosition({ 0.f, 0.f });
		SetSize({ float(Prism::Engine::GetInstance()->GetWindowSize().x), float(Prism::Engine::GetInstance()->GetWindowSize().y) });
	}

	GUIManager::~GUIManager()
	{
		delete myWidgets;
		myWidgets = nullptr;
		myActiveWidget = nullptr;
	}

	void GUIManager::AddWidget(const std::string& aName, Widget* aWidget)
	{
		myWidgets->AddWidget(aName, aWidget);
	}

	void GUIManager::Update()
	{
		myMousePosition = myInputWrapper->GetMousePosition();
		myMousePosition.y = float(Prism::Engine::GetInstance()->GetWindowSize().y) - myInputWrapper->GetMousePosition().y;

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

	Widget* GUIManager::FindWidget(const std::string& aWidgetName)
	{
		return myWidgets->FindWidget(aWidgetName);
	}
	
	void GUIManager::CheckMousePressed()
	{
		if (myActiveWidget != nullptr)
		{
			if (myInputWrapper->MouseIsPressed(0) == true)
			{
				myActiveWidget->OnMousePressed(myMousePosition);
			}
		}
	}
	
	void GUIManager::CheckMouseDown()
	{
		if (myActiveWidget != nullptr)
		{
			if (myInputWrapper->MouseDown(0) == true)
			{
				myActiveWidget->OnMouseDown(myMousePosition);
			}
		}
	}
	
	void GUIManager::CheckMouseReleased()
	{
		if (myActiveWidget != nullptr)
		{
			if (myInputWrapper->MouseUp(0) == true)
			{
				myActiveWidget->OnMouseUp();
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