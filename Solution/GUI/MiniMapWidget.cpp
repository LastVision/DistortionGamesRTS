#include "stdafx.h"

#include <EventPositionMessage.h>
#include "../Engine/Camera.h"
#include "../Entity/Entity.h"
#include "../Entity/PollingStation.h"
#include "MiniMapWidget.h"
#include <MinimapEventMessage.h>
#include <MinimapMoveMessage.h>
#include <MoveCameraMessage.h>
#include <PostMaster.h>
#include "../Entity/TriggerComponent.h"

namespace GUI
{
	MiniMapWidget::MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const Prism::Camera* aCamera
		, const bool& aCantClickOn, int aLevelID)
		: Widget()
		, myCantClickOn(aCantClickOn)
		, myShouldRenderEvent(false)
		, myEventTime(5.f)
	{
		std::string mapPath = "";
		std::string unitPath = "";
		std::string basePath = "";
		std::string resourcePath = "";
		std::string victoryPath = "";
		std::string artifactPath = "";
		std::string eventPath = "";
		std::string cameraPath = "";
		std::string levelID = "tutorial";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		if (aLevelID == 0)
		{
			levelID = "tutorial";
		}
		else if (aLevelID == 1)
		{
			levelID = "first";
		}

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "mapsprites"), levelID, mapPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unit"), "sprite", unitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "base"), "sprite", basePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "resourcepoints"), "sprite", resourcePath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "victorypoints"), "sprite", victoryPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "artifacts"), "sprite", artifactPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "event"), "sprite", eventPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "camera"), "sprite", cameraPath);

		myCameraOrientation = &aCamera->GetOrientation();

		myPlaceholderSprite = Prism::ModelLoader::GetInstance()->LoadSprite(mapPath, mySize);
		myUnitSprite = Prism::ModelLoader::GetInstance()->LoadSprite(unitPath, { 10.f, 10.f }, { 5.f, 5.f });
		myBaseSprite = Prism::ModelLoader::GetInstance()->LoadSprite(basePath, { 20.f, 20.f }, { 10.f, 10.f });
		myResourcePointSprite = Prism::ModelLoader::GetInstance()->LoadSprite(resourcePath, { 20.f, 20.f }, { 10.f, 10.f });
		myVictoryPointSprite = Prism::ModelLoader::GetInstance()->LoadSprite(victoryPath, { 20.f, 20.f }, { 10.f, 10.f });
		myArtifactSprite = Prism::ModelLoader::GetInstance()->LoadSprite(artifactPath, { 15.f, 15.f }, { 7.5f, 7.5f });
		myEventSprite = Prism::ModelLoader::GetInstance()->LoadSprite(eventPath, { 20.f, 20.f }, { 10.f, 10.f });

		CU::Vector2<float> cameraSize = { mySize.x / 2.f, mySize.y / 4.f };
		
		myCameraFrustum = Prism::ModelLoader::GetInstance()->LoadSprite(cameraPath, cameraSize, cameraSize / 2.f);

		PostMaster::GetInstance()->Subscribe(eMessageType::MINIMAP_EVENT, this);
	}

	MiniMapWidget::~MiniMapWidget()
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::MINIMAP_EVENT, this);
		SAFE_DELETE(myPlaceholderSprite);
		SAFE_DELETE(myUnitSprite);
		SAFE_DELETE(myBaseSprite);
		SAFE_DELETE(myResourcePointSprite);
		SAFE_DELETE(myVictoryPointSprite);
		SAFE_DELETE(myArtifactSprite);
		SAFE_DELETE(myCameraFrustum);
		SAFE_DELETE(myEventSprite);
	}
	
	void MiniMapWidget::Update(float aDelta)
	{
		if (myShouldRenderEvent == true)
		{
			myEventTimer += aDelta;
		}

		if (myEventTimer > myEventTime)
		{
			myShouldRenderEvent = false;
		}
	}

	void MiniMapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myPlaceholderSprite->Render(aParentPosition + myPosition);

		RenderUnits(aParentPosition);
		RenderResourcePoints(aParentPosition);
		RenderVictoryPoints(aParentPosition);
		RenderBases(aParentPosition);
		RenderArtifacts(aParentPosition);
		
		CU::Vector2<float> cameraPosition = { myCameraOrientation->GetPos().x, myCameraOrientation->GetPos().z };
		cameraPosition /= 255.f;
		cameraPosition *= mySize;
		cameraPosition.x += myCameraFrustum->GetSize().x / 4.f;
		cameraPosition.y += myCameraFrustum->GetSize().y * 1.4f;
		myCameraFrustum->Render(cameraPosition);
		
		if (myShouldRenderEvent == true)
		{
			//if (static_cast<int>(myEventTimer) % 2 == 0)
			{
				CU::Vector2<float> position = (myEventPosition / 255.f) * mySize;
				float scale = 2 + 5 * log(myEventTimer + 1);
				myEventSprite->Render(aParentPosition + myPosition + position, { scale, scale }, { 1.f, 1.f, 1.f, 1.f - (myEventTimer * 51.f / 255.f) });
			}
		}
	}

	void MiniMapWidget::OnMousePressed(const CU::Vector2<float>& aPosition)
	{
		if (myCantClickOn == false)
		{
			CU::Vector2<float> position = aPosition - myPosition;
			position /= mySize;
			PostMaster::GetInstance()->SendMessage(MoveCameraMessage(position,eHowToHandleMovement::ZERO_TO_ONE));
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

	void MiniMapWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);

		CU::Vector2<float> frustumRatio = myCameraFrustum->GetSize() / anOldWindowSize.x;

		myPlaceholderSprite->SetSize(mySize, { 0.f, 0.f });

		if (myIsFullscreen == false)
		{
			myCameraFrustum->SetSize(frustumRatio * aNewWindowSize.x, (frustumRatio * aNewWindowSize.x) / 2.f);
		}
		else
		{
			myCameraFrustum->SetSize(frustumRatio * aNewWindowSize, (frustumRatio * aNewWindowSize) / 2.f);
		}
	}

	void MiniMapWidget::ReceiveMessage(const MinimapEventMessage& aMessage)
	{
		aMessage.myMessageType;
		myEventPosition = aMessage.myPosition;
		myEventTimer = 0.f;
		myShouldRenderEvent = true;

		PostMaster::GetInstance()->SendMessage(EventPositionMessage(myEventPosition));
	}

	void MiniMapWidget::RenderUnits(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
		const CU::GrowingArray<Entity*>& enemyUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY);
		const CU::GrowingArray<Entity*>& neutralUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL);

		for (int i = 0; i < playerUnits.Size(); i++)
		{
			CU::Vector4<float> color = { 0.f, 0.f, 1.f, 1.f };
			if (playerUnits[i]->IsSelected() == true)
			{
				color = { 1.f, 1.f, 1.f, 1.f };
			}
			CU::Vector2<float> position = (playerUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, color);
		}

		for (int i = 0; i < enemyUnits.Size(); i++)
		{
			CU::Vector2<float> position = (enemyUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });
		}

		for (int i = 0; i < neutralUnits.Size(); i++)
		{
			CU::Vector2<float> position = (neutralUnits[i]->GetPosition() / 255.f) * mySize;
			myUnitSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, { 0.5f, 0.5f, 0.f, 1.f });
		}
	}

	void MiniMapWidget::RenderVictoryPoints(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& victoryPoints = PollingStation::GetInstance()->GetVictoryPoints();

		for (int i = 0; i < victoryPoints.Size(); i++)
		{
			CU::Vector2<float> position = (victoryPoints[i]->GetPosition() / 255.f) * mySize;
			CU::Vector4<float> color = { 0.5f, 0.5f, 0.f, 1.f };

			if (victoryPoints[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() == eOwnerType::PLAYER)
			{
				color = { 0.f, 0.f, 1.f, 1.f };
			}
			else if (victoryPoints[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() == eOwnerType::ENEMY)
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

			if (resourcePoints[i]->GetComponent<TriggerComponent>() != nullptr)
			{
				if (resourcePoints[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() == eOwnerType::PLAYER)
				{
					color = { 0.f, 0.f, 1.f, 1.f };
				}
				else if (resourcePoints[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() == eOwnerType::ENEMY)
				{
					color = { 1.f, 0.f, 0.f, 1.f };
				}

				myResourcePointSprite->Render(aParentPosition + myPosition + position, { 1.f, 1.f }, color);
			}
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

	void MiniMapWidget::RenderArtifacts(const CU::Vector2<float>& aParentPosition)
	{
		const CU::GrowingArray<Entity*>& artifacts = PollingStation::GetInstance()->GetArtifacts();

		for (int i = 0; i < artifacts.Size(); i++)
		{
			CU::Vector2<float> position = (artifacts[i]->GetPosition() / 255.f) * mySize;

			myArtifactSprite->Render(aParentPosition + myPosition + position);
		}
	}
}