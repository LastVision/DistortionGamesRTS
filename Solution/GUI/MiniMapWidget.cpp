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
		myUnitSprite = new Prism::Sprite("Data/Resource/Texture/UI/Minimap/T_minimap_unit.dds", { 10.f, 10.f });
		myBaseSprite = new Prism::Sprite("Data/Resource/Texture/UI/Minimap/T_minimap_base.dds", { 20.f, 20.f });
		myResourcePointSprite = new Prism::Sprite("Data/Resource/Texture/UI/Minimap/T_minimap_resource_point.dds", { 20.f, 20.f });

	}

	MiniMapWidget::~MiniMapWidget()
	{
		SAFE_DELETE(myPlaceholderSprite);
		SAFE_DELETE(myUnitSprite);
		SAFE_DELETE(myBaseSprite);
		SAFE_DELETE(myResourcePointSprite);
	}

	void MiniMapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myPlaceholderSprite->Render(aParentPosition + myPosition);

		const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
		const CU::GrowingArray<Entity*>& enemyUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY);
		const CU::GrowingArray<Entity*>& resourcePoints = PollingStation::GetInstance()->GetResourcePoints();

		for (int i = 0; i < playerUnits.Size(); i++)
		{
			CU::Vector2<float> position = (playerUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 0.f, 0.f, 1.f, 1.f});
		}

		for (int i = 0; i < enemyUnits.Size(); i++)
		{
			CU::Vector2<float> position = (enemyUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });
		}

		for (int i = 0; i < resourcePoints.Size(); i++)
		{
			CU::Vector2<float> position = (resourcePoints[i]->GetPosition() / 255.f) * mySize;
			CU::Vector4<float> color = { 0.5f, 0.5f, 0.f, 1.f };

			if (resourcePoints[i]->GetOwner() == eOwnerType::PLAYER)
			{
				color = { 0.f, 0.f, 1.f, 1.f };
			}
			else if (resourcePoints[i]->GetOwner() == eOwnerType::ENEMY)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}

			myResourcePointSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, color);
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