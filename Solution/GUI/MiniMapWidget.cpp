#include "stdafx.h"
#include "../Entity/Entity.h"
#include "../Entity/PollingStation.h"
#include "MiniMapWidget.h"
#include <MoveCameraMessage.h>
#include <Sprite.h>
#include <PostMaster.h>
#include <XMLReader.h>

namespace GUI
{
	MiniMapWidget::MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);

		mySize = size;
		myPosition = position;

		myPlaceholderSprite = new Prism::Sprite("Data/Resource/Texture/UI/Minimap/T_minimap_placeholder.dds", mySize);
		myFriendlyUnitSprite = new Prism::Sprite("Data/Resource/Texture/UI/Minimap/T_minimap_friendly_unit.dds", { 10.f, 10.f });
		myEnemyUnitSprite = new Prism::Sprite("Data/Resource/Texture/UI/Minimap/T_minimap_enemy_unit.dds", { 10.f, 10.f });

	}

	MiniMapWidget::~MiniMapWidget()
	{
		SAFE_DELETE(myPlaceholderSprite);
		SAFE_DELETE(myFriendlyUnitSprite);
		SAFE_DELETE(myEnemyUnitSprite);
	}

	void MiniMapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myPlaceholderSprite->Render(aParentPosition + myPosition);

		const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
		const CU::GrowingArray<Entity*>& enemyUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY);

		for (int i = 0; i < playerUnits.Size(); i++)
		{
			CU::Vector2<float> position = playerUnits[i]->GetPosition() / 255.f;
			position *= mySize;
			myFriendlyUnitSprite->Render(aParentPosition + myPosition + position);
		}

		for (int i = 0; i < enemyUnits.Size(); i++)
		{
			CU::Vector2<float> position = enemyUnits[i]->GetPosition() / 255.f;
			position *= mySize;
			myEnemyUnitSprite->Render(aParentPosition + myPosition + position);
		}
	}

	void MiniMapWidget::OnMousePressed(const CU::Vector2<float>& aPosition)
	{
		CU::Vector2<float> position = aPosition - myPosition;
		position /= mySize;
		PostMaster::GetInstance()->SendMessage(MoveCameraMessage(position));
	}

	void MiniMapWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		myPlaceholderSprite->SetSize(mySize, { 0.f, 0.f });
	}
}