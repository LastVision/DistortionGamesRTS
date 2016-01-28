#include "stdafx.h"
#include <Engine.h>
#include "TooltipInfo.h"
#include "TooltipWidget.h"
#include "GUIManager.h"

namespace GUI
{
	TooltipWidget::TooltipWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const GUIManager* aGuiManager)
		: Widget()
		, myGuiManager(aGuiManager)
		, myBackground(nullptr)
		, myCooldownSprite(nullptr)
		, myGunpowderSprite(nullptr)
		, mySupplySprite(nullptr)
		, myHeadlineScale(1.f)
		, myTextScale(1.f)
	{
		mySize = { 0.f, 0.f};
		myPosition = { 0.f, 0.f };

		std::string backgroundPath = "";
		std::string cooldownPath = "";
		std::string gunpowderPath = "";
		std::string supplyPath = "";
		std::string artifactPath = "";
		CU::Vector2<float> costSpriteSize;
		CU::Vector2<float> backgroundSize;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsize"), "x", backgroundSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsize"), "y", backgroundSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "headlineposition"), "x", myHeadlinePosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "headlineposition"), "y", myHeadlinePosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "x", myTextPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "y", myTextPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsprite"), "path", backgroundPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "costspritesize"), "x", costSpriteSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "costspritesize"), "y", costSpriteSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "costtextoffset"), "x", myCostTextOffset.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "costtextoffset"), "y", myCostTextOffset.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "cooldownsprite"), "path", cooldownPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gunpowdersprite"), "path", gunpowderPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "supplysprite"), "path", supplyPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "artifactsprite"), "path", artifactPath);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "headlinescale"), "value", myHeadlineScale);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textscale"), "value", myTextScale);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "cooldownsprite"), "positionx", myCooldownPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "cooldownsprite"), "positiony", myCooldownPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gunpowdersprite"), "positionx", myGunpowderPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gunpowdersprite"), "positiony", myGunpowderPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "supplysprite"), "positionx", mySupplyPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "supplysprite"), "positiony", mySupplyPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "artifactsprite"), "positionx", myArtifactPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "artifactsprite"), "positiony", myArtifactPosition.y);

		myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(backgroundPath, backgroundSize);
		myCooldownSprite = Prism::ModelLoader::GetInstance()->LoadSprite(cooldownPath, costSpriteSize);
		myGunpowderSprite = Prism::ModelLoader::GetInstance()->LoadSprite(gunpowderPath, costSpriteSize);
		mySupplySprite = Prism::ModelLoader::GetInstance()->LoadSprite(supplyPath, costSpriteSize);
		myArtifactSprite = Prism::ModelLoader::GetInstance()->LoadSprite(artifactPath, costSpriteSize);
	}

	TooltipWidget::~TooltipWidget()
	{
		SAFE_DELETE(myBackground);
		SAFE_DELETE(myCooldownSprite);
		SAFE_DELETE(myGunpowderSprite);
		SAFE_DELETE(mySupplySprite);
		SAFE_DELETE(myArtifactSprite);
	}

	void TooltipWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myActiveWidget = myGuiManager->GetActiveWidget();

		if (myActiveWidget != nullptr && myActiveWidget->GetTooltipInfo() != nullptr)
		{
			const TooltipInfo* tooltipInfo = myActiveWidget->GetTooltipInfo();
			CU::Vector2<float> position = myPosition + aParentPosition;

			myBackground->Render(position);

			Prism::Engine::GetInstance()->PrintText(tooltipInfo->myHeadline, myHeadlinePosition + position
				, Prism::eTextType::RELEASE_TEXT, myHeadlineScale);

			Prism::Engine::GetInstance()->PrintText(tooltipInfo->myText, myTextPosition + position
				, Prism::eTextType::RELEASE_TEXT, myTextScale);

			//if (tooltipInfo->myCooldown != -1)
			//{
				myCooldownSprite->Render(position + myCooldownPosition);
			//	Prism::Engine::GetInstance()->PrintText(tooltipInfo->myCooldown, position + myCooldownPosition + myCostTextOffset
			//		, Prism::eTextType::RELEASE_TEXT, myTextScale);
			//}
				myGunpowderSprite->Render(position + myGunpowderPosition);
				mySupplySprite->Render(position + mySupplyPosition);
				myArtifactSprite->Render(position + myArtifactPosition);
		}
	}

	void TooltipWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);

		if (myIsFullscreen == false)
		{
			myHeadlinePosition = (myHeadlinePosition / anOldWindowSize.x) * aNewWindowSize.x;
			myTextPosition = (myTextPosition / anOldWindowSize.x) * aNewWindowSize.x;

			CU::Vector2<float> backgroundSize = myBackground->GetSize() / anOldWindowSize.x;
			myBackground->SetSize(backgroundSize * aNewWindowSize.x, { 0.f, 0.f });
		}
		else
		{
			myHeadlinePosition = (myHeadlinePosition / anOldWindowSize) * aNewWindowSize;
			myTextPosition = (myTextPosition / anOldWindowSize) * aNewWindowSize;

			CU::Vector2<float> backgroundSize = myBackground->GetSize() / anOldWindowSize;
			myBackground->SetSize(backgroundSize * aNewWindowSize, { 0.f, 0.f });
		}

		myTextScale = (myTextScale / anOldWindowSize.x) * aNewWindowSize.x;
		myHeadlineScale = (myHeadlineScale / anOldWindowSize.x) * aNewWindowSize.x;
	}
}
