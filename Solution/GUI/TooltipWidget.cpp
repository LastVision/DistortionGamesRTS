#include "stdafx.h"
#include <Engine.h>
#include "TooltipWidget.h"
#include "GUIManager.h"

struct TooltipInfo
{
	std::string myHeadline;
	std::string myText;
	int myGoldCost;
	int myArftifactCost;
	int mySupplyCost;
	float myCooldown;
};


namespace GUI
{
	TooltipWidget::TooltipWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const GUIManager* aGuiManager)
		: Widget()
		, myGuiManager(aGuiManager)
		, myBackground(nullptr)
		, myTextScale(1.f)
	{
		mySize = { 0.f, 0.f };

		std::string backgroundPath;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsize"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsize"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "x", myTextPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "y", myTextPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsprite"), "path", backgroundPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textscale"), "value", myTextScale);

		myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(backgroundPath, mySize);
	}

	TooltipWidget::~TooltipWidget()
	{
		SAFE_DELETE(myBackground);
	}

	void TooltipWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myActiveWidget = myGuiManager->GetActiveWidget();

		if (myActiveWidget != nullptr && myActiveWidget->GetHoverText() != "")
		{
			myBackground->Render(myPosition + aParentPosition);
			Prism::Engine::GetInstance()->PrintText(myActiveWidget->GetHoverText(), myTextPosition + myPosition + aParentPosition
				, Prism::eTextType::RELEASE_TEXT, myTextScale);
		}
	}

	Widget* TooltipWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		aPosition;
		return nullptr;
	}

	void TooltipWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);

		myTextScale = (myTextScale / anOldWindowSize.x) * aNewWindowSize.x;
	}
}
