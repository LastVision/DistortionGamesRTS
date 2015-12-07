#include "stdafx.h"

#include <BuildingComponent.h>
#include <Camera.h>
#include <CollisionComponent.h>
#include <ControllerComponent.h>
#include <EntityFactory.h>
#include <GUIManager.h>
#include <Intersection.h>
#include <InputWrapper.h>
#include <PathFinder.h>
#include "PlayerDirector.h"
#include <PollingStation.h>
#include <Terrain.h>
#include <ToggleGUIMessage.h>
#include <ModelLoader.h>
#include <SpawnUnitMessage.h>

#include <FadeMessage.h>
#include <PostMaster.h>

PlayerDirector::PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor)
	: Director(eDirectorType::PLAYER, aTerrain)
	, myRenderGUI(true)
	, myCursor(aCursor)
	, myGUIManager(nullptr)
	, mySelectedUnits(56)
{
	for (int i = 0; i < 64; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			aScene, { 65, 0, 40 }, aTerrain));
		
	}
	
	Prism::ModelLoader::GetInstance()->Pause();
	myGUIManager = new GUI::GUIManager(aCursor, "Data/Resource/GUI/GUI_ingame.xml", this);
	Prism::ModelLoader::GetInstance()->UnPause();

	myActiveUnits.Add(myUnits[0]);
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		myActiveUnits[i]->Spawn({ 65.f, 0.f, 25.f });
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits[i]);
	}

	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_GUI, this);

}

PlayerDirector::~PlayerDirector()
{
	SAFE_DELETE(myGUIManager);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_GUI, this);
}

void PlayerDirector::Update(float aDeltaTime, const Prism::Camera& aCamera)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_G) == true)
	{
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(!myRenderGUI, 1.f/3.f));
	}

	UpdateInputs();

	Director::Update(aDeltaTime);
	UpdateMouseInteraction(aCamera);


	////Debug only --- (LinusL)
	//CU::Vector3<float> goal = CalcCursorWorldPosition(aCamera);
	//CU::GrowingArray<Prism::Navigation::Triangle*> path(16);
	//if (myTerrain.GetPathFinder()->FindPath({ 1.f, 30.f, 2.f }, goal, path) == true)
	//{
	//	if (path.Size() > 0)
	//	{
	//		Prism::RenderLine3D({ 1.f, 30.f, 2.f }, path.GetLast()->GetCenter(), eColorDebug::WHITE);
	//		Prism::RenderLine3D(goal, path[0]->GetCenter(), eColorDebug::WHITE);
	//	}
	//	for (int i = 0; i < path.Size() - 1; ++i)
	//	{
	//		Prism::RenderLine3D(path[i]->GetCenter(), path[i + 1]->GetCenter(), eColorDebug::WHITE);
	//	}
	//}
	//// --- debug end

	myBuilding->Update(aDeltaTime);

	if (myRenderGUI == true)
	{
		myGUIManager->Update();
	}
}

void PlayerDirector::Render()
{
	if (myRenderGUI == true)
	{
		myGUIManager->Render();
	}
}

void PlayerDirector::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void PlayerDirector::SpawnUnit(Prism::Scene& aScene)
{
	myBuilding->GetComponent<BuildingComponent>()->BuildUnit(eEntityType::DRAGON);
}

void PlayerDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (aMessage.myOwnerType != static_cast<int>(eOwnerType::PLAYER)) return;
	if (myActiveUnits.Size() < 64)
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetType() == static_cast<eEntityType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
			{
				myUnits[i]->Spawn(myBuilding->GetOrientation().GetPos() + CU::Vector3f(0.f, 0.f, -15.f));
				myActiveUnits.Add(myUnits[i]);
				break;
			}
		}
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits.GetLast());
	}
}

void PlayerDirector::ReceiveMessage(const ToggleGUIMessage& aMessage)
{
	myRenderGUI = aMessage.myShowGUI;
	PostMaster::GetInstance()->SendMessage(FadeMessage(aMessage.myFadeTime));
}

const BuildingComponent& PlayerDirector::GetBuildingComponent() const
{
	return *myBuilding->GetComponent<BuildingComponent>();
}

CU::Vector3<float> PlayerDirector::GetCameraMoveVector() const
{
	return myGUIManager->CalcCameraMovement();
}

void PlayerDirector::SelectUnit(Entity* anEntity)
{
	if (mySelectedUnits.Size() > 0 && mySelectedUnits[0]->GetType() != anEntity->GetType())
	{
		return;
	}

	for (int i = 0; i < mySelectedUnits.Size(); i++)
	{
		if (mySelectedUnits[i] == anEntity)
		{
			return;
		}
	}

	anEntity->SetSelect(true);
	mySelectedUnits.Add(anEntity);
}

CU::Vector3<float> PlayerDirector::CalcCursorWorldPosition(const Prism::Camera& aCamera)
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

void PlayerDirector::UpdateInputs()
{
	myShiftPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT)
		|| CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RSHIFT);
	myAPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A);
	mySPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S);
	if (myRenderGUI == true) // no inworld clicking when mouse is over gui:
	{
		myLeftMouseClicked = CU::InputWrapper::GetInstance()->MouseDown(0) && !(myGUIManager->MouseOverGUI());
		myRightClicked = CU::InputWrapper::GetInstance()->MouseDown(1) && !(myGUIManager->MouseOverGUI());
	}
	else
	{
		myLeftMouseClicked = CU::InputWrapper::GetInstance()->MouseDown(0);
		myRightClicked = CU::InputWrapper::GetInstance()->MouseDown(1);
	}


	if (myLeftMouseClicked == true && myShiftPressed == false && myAPressed == false)
	{
		mySelectedUnits.RemoveAll();
	}
}

void PlayerDirector::UpdateMouseInteraction(const Prism::Camera& aCamera)
{
	CU::Vector3<float> targetPos = CalcCursorWorldPosition(aCamera);
	Entity* hoveredEnemy = PollingStation::GetInstance()->FindEntityAtPosition(targetPos, eOwnerType::ENEMY);
	if (hoveredEnemy != nullptr)
	{
		Prism::RenderBox(hoveredEnemy->GetOrientation().GetPos(), eColorDebug::RED);
	}

	bool hasSelected = false;
	bool hasHovered = false;
	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), targetPos);
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		SelectOrHoverEntity(myUnits[i], hasSelected, hasHovered, line);
		ControllerComponent* controller = myUnits[i]->GetComponent<ControllerComponent>();

		if (myUnits[i]->IsSelected())
		{
			if (myAPressed && myLeftMouseClicked)
			{
				controller->Attack(targetPos, !myShiftPressed);
			}
			else if (myRightClicked)
			{
				//controller->MoveTo(targetPos, !myShiftPressed);
				controller->Stop();
				CU::GrowingArray<Prism::Navigation::Triangle*> path(16);
				if (myTerrain.GetPathFinder()->FindPath(myUnits[i]->GetOrientation().GetPos(), targetPos, path) == true)
				{
					for (int i = path.Size() - 1; i >= 0; --i)
					{
						CU::Vector3<float> target = path[i]->GetCenter();
						target.y = 0.f;
						controller->MoveTo(target, false);
					}
				}
			}
			else if (mySPressed)
			{
				controller->Stop();
			}
		}
	}


	SelectOrHoverEntity(myBuilding, hasSelected, hasHovered, line);
}

void PlayerDirector::SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
	, const CU::Intersection::LineSegment3D& aMouseRay)
{
	if (myLeftMouseClicked == true && myShiftPressed == false && myAPressed == false)
	{
		aEntity->SetSelect(false);
	}

	aEntity->SetHovered(false);

	if (aEntity->GetComponent<CollisionComponent>()->Collide(aMouseRay))
	{
		if (myLeftMouseClicked == true && aSelected == false)
		{
			SelectUnit(aEntity);
			aSelected = true;
		}
		else if (aHovered == false)
		{
			aEntity->SetHovered(true);
			aHovered = true;
		}
	}
}