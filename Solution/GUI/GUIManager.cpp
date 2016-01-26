#include "stdafx.h"
#include "AbilityButton.h"
#include "ButtonWidget.h"
#include "ControlGroupWidget.h"
#include "Cursor.h"
#include <Engine.h>
#include "../Game/PlayerDirector.h"
#include "GUIManager.h"
#include "../InputWrapper/InputWrapper.h"
#include "MiniMapWidget.h"
#include "ResourceBarWidget.h"
#include "SpriteWidget.h"
#include "TextWidget.h"
#include "TooltipWidget.h"
#include "UnitActionWidget.h"
#include "UnitCapWidget.h"
#include "UnitInfoWidget.h"
#include "UpgradeButtonWidget.h"
#include "WidgetContainer.h"

namespace GUI
{
	GUIManager::GUIManager(Cursor* aCursor, const std::string& aXMLPath, const PlayerDirector* aPlayer
		, const AIDirector* anAI, const Prism::Camera* aCamera, int aLeveID)
		: myActiveWidget(nullptr)
		, myCursor(aCursor)
		, myPlayer(aPlayer)
		, myAI(anAI)
		, myMousePosition({ 0.f, 0.f })
		, myCamera(aCamera)
		, myLevelID(aLeveID)
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

	void GUIManager::Update(float aDelta)
	{
		myMousePosition = myCursor->GetMousePosition();

		myWidgets->Update(aDelta);

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
		myWidgets->OnResize(newSize, myWindowSize, true);
		myWindowSize = newSize;
	}

	CU::Vector3<float> GUIManager::CalcCameraMovement() const
	{
		CU::Vector3<float> movement;

		float epsilon = 0.01f;
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

		myWidgets = new WidgetContainer(nullptr, myWindowSize, true);
		myWidgets->SetPosition({ 0.f, 0.f });

		tinyxml2::XMLElement* containerElement = reader.ForceFindFirstChild(rootElement, "container");
		for (; containerElement != nullptr; containerElement = reader.FindNextElement(containerElement))
		{
			bool isFullscreen = false;
			bool isClickable = true;
			Prism::SpriteProxy* backgroundSprite = nullptr;

			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "size"), "x", size.x);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "size"), "y", size.y);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "position"), "x", position.x);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(containerElement, "position"), "y", position.y);

			tinyxml2::XMLElement* spriteElement = reader.FindFirstChild(containerElement, "backgroundsprite");
			tinyxml2::XMLElement* spriteSizeElement = reader.FindFirstChild(containerElement, "backgroundsize");
			tinyxml2::XMLElement* fullscreenElement = reader.FindFirstChild(containerElement, "isfullscreen");
			tinyxml2::XMLElement* clickableElement = reader.FindFirstChild(containerElement, "isclickable");

			if (spriteElement != nullptr)
			{
				reader.ForceReadAttribute(spriteElement, "path", path);

				if (spriteSizeElement != nullptr)
				{
					CU::Vector2<float> spriteSize;
					reader.ForceReadAttribute(spriteSizeElement, "x", spriteSize.x);
					reader.ForceReadAttribute(spriteSizeElement, "y", spriteSize.y);
					backgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, spriteSize);
				}
				else
				{
					backgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(path, size);
				}
			}

			if (fullscreenElement != nullptr)
			{
				reader.ForceReadAttribute(fullscreenElement, "value", isFullscreen);
			}

			if (clickableElement != nullptr)
			{
				reader.ForceReadAttribute(clickableElement, "value", isClickable);
			}

			GUI::WidgetContainer* container = new WidgetContainer(backgroundSprite, size, isFullscreen);
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
					UnitActionWidget* unitActions = new UnitActionWidget(&reader, widgetElement, myPlayer->GetSelectedUnits(), myPlayer);
					container->AddWidget(unitActions);
				}
				else if (type == "minimap")
				{
					MiniMapWidget* minimap = new MiniMapWidget(&reader, widgetElement, myCamera, myPlayer->GetRenderDragSelection(), myLevelID);
					container->AddWidget(minimap);
				}
				else if (type == "resourcebar")
				{
					ResourceBarWidget* resourceBar = new ResourceBarWidget(&reader, widgetElement, myPlayer, myAI);
					container->AddWidget(resourceBar);
				}
				else if (type == "control_groups")
				{
					ControlGroupWidget* controlGroup = new ControlGroupWidget(&reader, widgetElement, myPlayer, size);
					container->AddWidget(controlGroup);
				}
				else if (type == "text")
				{
					TextWidget* textWidget = new TextWidget(&reader, widgetElement);
					container->AddWidget(textWidget);
				}
				else if (type == "unit_cap")
				{
					UnitCapWidget* unitCapWidget = new UnitCapWidget(&reader, widgetElement, size, myPlayer->GetUnitCap()
						, myPlayer->GetUnitCount());
					container->AddWidget(unitCapWidget);
				}
				else if (type == "upgrade_button")
				{
					UpgradeButtonWidget* upgradeButtonWidget = new UpgradeButtonWidget(&reader, widgetElement, myPlayer);
					container->AddWidget(upgradeButtonWidget);
				}
				else if (type == "ability_button")
				{
					AbilityButton* upgradeButtonWidget = new AbilityButton(&reader, widgetElement, myPlayer);
					container->AddWidget(upgradeButtonWidget);
				}
				else if (type == "tool_tip")
				{
					TooltipWidget* tooltipWidget = new TooltipWidget(&reader, widgetElement, this);
					container->AddWidget(tooltipWidget);
				}
				else if (type == "sprite")
				{
					SpriteWidget* spriteWidget = new SpriteWidget(&reader, widgetElement);
					container->AddWidget(spriteWidget);
				}
				else
				{
					// should assert when all widgets are in
				}
			}
			container->SetIsClickable(isClickable);
			myWidgets->AddWidget(container);
		}

		reader.CloseDocument();

		OnResize(Prism::Engine::GetInstance()->GetWindowSizeInt().x, Prism::Engine::GetInstance()->GetWindowSizeInt().y);
	}

	void GUIManager::CheckMousePressed()
	{
		if (myActiveWidget != nullptr)
		{
			if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) == true)
			{
				myActiveWidget->OnMousePressed(myMousePosition);
			}
			if (CU::InputWrapper::GetInstance()->MouseIsPressed(1) == true)
			{
				myActiveWidget->OnRightMousePressed(myMousePosition);
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
			if (CU::InputWrapper::GetInstance()->MouseDown(1) == true)
			{
				myActiveWidget->OnRightMouseDown(myMousePosition);
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

		if (activeWidget != nullptr && activeWidget->IsClickable() == false)
		{
			return;
		}

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