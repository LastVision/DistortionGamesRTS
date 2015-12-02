#include "stdafx.h"

#include <Camera.h>
#include <CollisionComponent.h>
#include <EntityFactory.h>
#include <GUIManager.h>
#include <Intersection.h>
#include <InputWrapper.h>
#include <MovementComponent.h>
#include "PlayerActor.h"
#include <Terrain.h>

PlayerActor::PlayerActor(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor)
	: Actor(eActorType::PLAYER, aTerrain)
	, myRenderGUI(true)
	, myCursor(aCursor)
	, myGUIManager(nullptr)
{
	for (int i = 0; i < 15; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 20.f }, aTerrain));
	}
	myGUIManager = new GUI::GUIManager(aCursor, "Data/Resource/GUI/GUI_ingame.xml", myUnits);
}

PlayerActor::~PlayerActor()
{
	SAFE_DELETE(myGUIManager);
}

void PlayerActor::Update(float aDeltaTime, const Prism::Camera& aCamera)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_G) == true)
	{
		myRenderGUI = !myRenderGUI;
	}

	Actor::Update(aDeltaTime);
	UpdateMouseInteraction(aCamera);

	if (myRenderGUI == true)
	{
		myGUIManager->Update();
	}
}

void PlayerActor::Render()
{
	if (myRenderGUI == true)
	{
		myGUIManager->Render();
	}
}

void PlayerActor::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

CU::Vector3<float> PlayerActor::CalcCursorWorldPosition(const Prism::Camera& aCamera)
{
	CU::Vector2<float> inputPos(CU::InputWrapper::GetInstance()->GetMousePosition());
	CU::Vector2<float> cursorPos;
	CU::Vector2<float> window = Prism::Engine::GetInstance()->GetWindowSize();

	float tweakValue = 1.85f; // for 16:9
	float epsilon = 0.1f;
	float aspect = window.x / window.y;
	if (aspect <= 5.f / 4.f + epsilon)
	{
		tweakValue = 1.255f;
	}
	else if (aspect <= 16.f / 10.f + epsilon)
	{
		tweakValue = 1.605f;
	}

	float padding = (window.x - window.y) * 0.5f;
	float mult = window.y / window.x;
	cursorPos = inputPos;

	cursorPos.y = window.y - cursorPos.y;
	cursorPos.y /= window.y;
	cursorPos.y *= mult / tweakValue;
	cursorPos.y += (1.f - mult / tweakValue) / 2.f;

	cursorPos.x /= window.x;

	Prism::Engine::GetInstance()->PrintText(cursorPos.x, { 50.f, 50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(cursorPos.y, { 280.f, 50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(tweakValue, { 480.f, 50.f }, Prism::eTextType::DEBUG_TEXT);

	CU::Vector3<float> worldPos(myTerrain.CalcIntersection(aCamera.GetOrientation().GetPos()
		, aCamera.RayCast(cursorPos)));

	//Debug:
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });

	return worldPos;
}

void PlayerActor::UpdateMouseInteraction(const Prism::Camera& aCamera)
{
	CU::Vector3<float> targetPos = CalcCursorWorldPosition(aCamera);
	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), targetPos);

	bool leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0);
	bool hasSelected = false;
	bool hasHovered = false;
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		bool mouseOnUnit = myUnits[i]->GetComponent<CollisionComponent>()->Collide(line);

		if (leftClicked == true)
		{
			myUnits[i]->SetSelect(false);
		}

		myUnits[i]->SetHovered(false);

		if (mouseOnUnit == true)
		{
			if (leftClicked == true && hasSelected == false)
			{
				myUnits[i]->SetSelect(true);
				hasSelected = true;
			}
			else if (hasHovered == false)
			{
				myUnits[i]->SetHovered(true);
				hasHovered = true;
			}
		}
	}

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) && CU::InputWrapper::GetInstance()->MouseDown(1))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->IsSelected())
			{
				myUnits[i]->GetComponent<MovementComponent>()->AddWayPoint(newPos, false);
			}
		}
	}
	else if (CU::InputWrapper::GetInstance()->MouseDown(1))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->IsSelected())
			{
				myUnits[i]->GetComponent<MovementComponent>()->AddWayPoint(newPos, true);
			}
		}
	}
}