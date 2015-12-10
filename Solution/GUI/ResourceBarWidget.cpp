#include "stdafx.h"
#include <Engine.h>
#include <Sprite.h>
#include "ResourceBarWidget.h"
#include "../Game/PlayerDirector.h"
#include <XMLReader.h>

namespace GUI
{
	ResourceBarWidget::ResourceBarWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: myValueSprite(nullptr)
	{
		std::string valueSpritePath = "";
		std::string value = "";
		CU::Vector2<float> size;
		CU::Vector2<float> position;
		CU::Vector2<float> textposition;
		CU::Vector2<float> valueSpriteSize;
		CU::Vector2<float> valueSpritePosition;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "value"), "type", value);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "value"), "positionx", textposition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "value"), "positiony", textposition.y);

		tinyxml2::XMLElement* spriteElement = aReader->FindFirstChild(anXMLElement, "valuesprite");
		if (spriteElement != nullptr)
		{
			aReader->ForceReadAttribute(spriteElement, "path", valueSpritePath);
			aReader->ForceReadAttribute(spriteElement, "sizex", valueSpriteSize.x);
			aReader->ForceReadAttribute(spriteElement, "sizey", valueSpriteSize.y);
			aReader->ForceReadAttribute(spriteElement, "positionx", valueSpritePosition.x);
			aReader->ForceReadAttribute(spriteElement, "positiony", valueSpritePosition.y);
			myValueSprite = new Prism::Sprite(valueSpritePath, valueSpriteSize);
		}

		if (value == "player_gold")
		{
			myValue = &aPlayer->GetTestGold();
		}

		mySize = size;
		myPosition = position;
		myTextPosition = textposition;
	}

	ResourceBarWidget::~ResourceBarWidget()
	{
		SAFE_DELETE(myValueSprite);
	}

	void ResourceBarWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myValueSprite != nullptr)
		{
			myValueSprite->Render(myPosition + aParentPosition);
		}

		Prism::Engine::GetInstance()->PrintText(*myValue, myPosition + aParentPosition + myTextPosition, Prism::eTextType::RELEASE_TEXT);
	}
}