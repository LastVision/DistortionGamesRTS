#include "stdafx.h"
#include "../Engine/Camera.h"
#include "../Entity/Entity.h"
#include "../Entity/PollingStation.h"
#include "MiniMapWidget.h"
#include <MinimapMoveMessage.h>
#include <MoveCameraMessage.h>
#include <PostMaster.h>

namespace GUI
{
	MiniMapWidget::MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const Prism::Camera* aCamera
		, const bool& aCantClickOn)
		: myCantClickOn(aCantClickOn)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);

		mySize = size;
		myPosition = position;
		myCameraOrientation = &aCamera->GetOrientation();

		myPlaceholderSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Minimap/T_minimap_placeholder.dds", mySize);
		myUnitSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Minimap/T_minimap_unit.dds", { 10.f, 10.f }, { 5.f, 5.f });
		myBaseSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Minimap/T_minimap_base.dds", { 20.f, 20.f }, { 10.f, 10.f });
		myResourcePointSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Minimap/T_minimap_resource_point.dds", { 20.f, 20.f }, { 10.f, 10.f });
		myVictoryPointSprite = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Minimap/T_minimap_victory_point.dds", { 20.f, 20.f }, { 10.f, 10.f });
		myCameraFrustum = Prism::ModelLoader::GetInstance()->LoadSprite(
			"Data/Resource/Texture/UI/Minimap/T_minimap_camera.dds", { 100.f, 50.f }, { 0.f, 0.f });
	}

	MiniMapWidget::~MiniMapWidget()
	{
		SAFE_DELETE(myPlaceholderSprite);
		SAFE_DELETE(myUnitSprite);
		SAFE_DELETE(myBaseSprite);
		SAFE_DELETE(myResourcePointSprite);
		SAFE_DELETE(myVictoryPointSprite);
		SAFE_DELETE(myCameraFrustum);
	}

	void MiniMapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myPlaceholderSprite->Render(aParentPosition + myPosition);

		RenderUnits(aParentPosition);
		RenderResourcePoints(aParentPosition);
		RenderVictoryPoints(aParentPosition);
		RenderBases(aParentPosition);
		
		CU::Vector2<float> cameraPosition = { myCameraOrientation->GetPos().x, myCameraOrientation->GetPos().z };
		cameraPosition /= 255.f;
		cameraPosition *= mySize;
		cameraPosition.x += myCameraFrustum->GetSize().x / 4.f;
		cameraPosition.y += myCameraFrustum->GetSize().y;
		myCameraFrustum->Render(cameraPosition);
	}

	void MiniMapWidget::OnMousePressed(const CU::Vector2<float>& aPosition)
	{
		if (myCantClickOn == false)
		{
			CU::Vector2<float> position = aPosition - myPosition;
			position /= mySize;
			PostMaster::GetInstance()->SendMessage(MoveCameraMessage(position));
		}
	}

	void MiniMapWidget::OnRightMouseDown(const CU::Vector2<float>& aPosition)
	{
		if (myCantClickOn == false)
		{
			CU::Vector2<float> position = aPosition - myPosition;
			position /= mySize;
			PostMaster::GetInstance()->SendMessage(MinimapMoveMessage(position));
		}
	}

	void MiniMapWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);

		CU::Vector2<float> frustumRatio = myCameraFrustum->GetSize() / anOldWindowSize;

		myPlaceholderSprite->SetSize(mySize, { 0.f, 0.f });
		myCameraFrustum->SetSize(frustumRatio * aNewWindowSize, { 0.f, 0.f });
	}

	void MiniMapWidget::RenderUnits(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
		const CU::GrowingArray<Entity*>& enemyUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY);

		for (int i = 0; i < playerUnits.Size(); i++)
		{
			CU::Vector2<float> position = (playerUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 0.f, 0.f, 1.f, 1.f });
		}

		for (int i = 0; i < enemyUnits.Size(); i++)
		{
			CU::Vector2<float> position = (enemyUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });
		}
	}

	void MiniMapWidget::RenderVictoryPoints(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& victoryPoints = PollingStation::GetInstance()->GetVictoryPoints();

		for (int i = 0; i < victoryPoints.Size(); i++)
		{
			CU::Vector2<float> position = (victoryPoints[i]->GetPosition() / 255.f) * mySize;
			CU::Vector4<float> color = { 0.5f, 0.5f, 0.f, 1.f };

			if (victoryPoints[i]->GetOwner() == eOwnerType::PLAYER)
			{
				color = { 0.f, 0.f, 1.f, 1.f };
			}
			else if (victoryPoints[i]->GetOwner() == eOwnerType::ENEMY)
			{
				color = { 1.f, 0.f, 0.f, 1.f };
			}

			myVictoryPointSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, color);
		}
	}

	void MiniMapWidget::RenderResourcePoints(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& resourcePoints = PollingStation::GetInstance()->GetResourcePoints();

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

	void MiniMapWidget::RenderBases(const CU::Vector2<float>& aParentPosition)
	{
		const Entity* enemyBase = PollingStation::GetInstance()->GetBase(eOwnerType::ENEMY);
		const Entity* playerBase = PollingStation::GetInstance()->GetBase(eOwnerType::PLAYER);

		CU::Vector2<float> basePosition = (enemyBase->GetPosition() / 255.f) * mySize;
		myBaseSprite->Render(aParentPosition + myPosition + basePosition, { 1.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });

		basePosition = (playerBase->GetPosition() / 255.f) * mySize;
		myBaseSprite->Render(aParentPosition + myPosition + basePosition, { 1.f, 1.f }, { 0.f, 0.f, 1.f, 1.f });
	}
}