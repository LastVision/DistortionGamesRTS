#include "stdafx.h"
#include <Engine.h>
#include <Sprite.h>
#include "ResourceBarWidget.h"
#include <XMLReader.h>

namespace GUI
{
	ResourceBarWidget::ResourceBarWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int& someGold)
		: myPlayerGold(someGold)
		, myGoldSprite(nullptr)
	{
		std::string goldSpritePath = "";
		CU::Vector2<float> goldSpriteSize;
		CU::Vector2<float> goldSpritePosition;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "goldsprite"), "path", goldSpritePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "goldsprite"), "sizex", goldSpriteSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "goldsprite"), "sizey", goldSpriteSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "goldsprite"), "positionx", goldSpritePosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "goldsprite"), "positiony", goldSpritePosition.y);

		mySize = goldSpriteSize;
		myPosition = goldSpritePosition;
		myGoldSprite = new Prism::Sprite(goldSpritePath, goldSpriteSize);
	}

	ResourceBarWidget::~ResourceBarWidget()
	{
		SAFE_DELETE(myGoldSprite);
	}

	void ResourceBarWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myGoldSprite->Render(myPosition + aParentPosition);

		CU::Vector2<float> position = myPosition + aParentPosition;
		position.x += myGoldSprite->GetSize().x;
		position.y += myGoldSprite->GetSize().y / 2.f;
		Prism::Engine::GetInstance()->PrintText(myPlayerGold, position, Prism::eTextType::RELEASE_TEXT);
	}
}