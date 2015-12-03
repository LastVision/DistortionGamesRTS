#include "stdafx.h"

#include <BuildingComponent.h>
#include <Camera.h>
#include <CollisionComponent.h>
#include <ControllerComponent.h>
#include <EntityFactory.h>
#include <GUIManager.h>
#include <Intersection.h>
#include <InputWrapper.h>
#include "PlayerDirector.h"
#include <PollingStation.h>
#include <Terrain.h>
#include <ModelLoader.h>
#include <SpawnUnitMessage.h>


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
			aScene, { 20.f + i, 0.f, 20.f }, aTerrain));
	}
	myBuilding = EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::BASE_BUILING, Prism::eOctreeType::STATIC, aScene, { 30, 0, 40 }, aTerrain);
	myBuilding->AddToScene();
	myBuilding->Reset();
	Prism::ModelLoader::GetInstance()->Pause();
	myGUIManager = new GUI::GUIManager(aCursor, "Data/Resource/GUI/GUI_ingame.xml", mySelectedUnits);
	Prism::ModelLoader::GetInstance()->UnPause();

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		PollingStation::GetInstance()->RegisterEntity(myUnits[i]);
	}
}

PlayerDirector::~PlayerDirector()
{
	SAFE_DELETE(myGUIManager);
}

void PlayerDirector::Update(float aDeltaTime, const Prism::Camera& aCamera)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_G) == true)
	{
		myRenderGUI = !myRenderGUI;
	}

	Director::Update(aDeltaTime);
	UpdateMouseInteraction(aCamera);

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

	//if (myUnits.Size() < 64)
	//{
	//	myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
	//		aScene, { 20.f, 0.f, 20.f }, myTerrain));
	//	PollingStation::GetInstance()->RegisterEntity(myUnits.GetLast());
	//}
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
				myUnits[i]->Spawn(myBuilding->GetOrientation().GetPos() + CU::Vector3f(2.f, 0.f, 2.f));
				myActiveUnits.Add(myUnits[i]);
				break;
			}
		}
		PollingStation::GetInstance()->RegisterEntity(myUnits.GetLast());
	}
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

void PlayerDirector::UpdateMouseInteraction(const Prism::Camera& aCamera)
{
	CU::Vector3<float> targetPos = CalcCursorWorldPosition(aCamera);
	Entity* hoveredEnemy = PollingStation::GetInstance()->FindEntityAtPosition(targetPos, eOwnerType::ENEMY);
	if (hoveredEnemy != nullptr)
	{
		Prism::RenderBox(hoveredEnemy->GetOrientation().GetPos(), eColorDebug::RED);
	}

	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), targetPos);

	bool hasSelected = false;
	bool hasHovered = false;
	bool shiftPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT)
		|| CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RSHIFT);
	bool leftClicked;
	if (myRenderGUI == true) // no inworld clicking when mouse is over gui:
	{
		leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0) && !(myGUIManager->MouseOverGUI());
	}
	else
	{
		leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0);
	}

	if (leftClicked == true && shiftPressed == false)
	{
		mySelectedUnits.RemoveAll();
	}

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		SelectOrHoverEntity(myUnits[i], hasSelected, hasHovered, line);

		if (CU::InputWrapper::GetInstance()->MouseDown(1))
		{
			if (myUnits[i]->IsSelected())
			{
				ControllerComponent* controller = myUnits[i]->GetComponent<ControllerComponent>();
				if (hoveredEnemy == nullptr)
				{
					if (shiftPressed == true)
					{
						controller->MoveTo(targetPos, false);
					}
					else
					{
						controller->AttackMove(targetPos);
					}
				}
				else
				{
					controller->Attack(hoveredEnemy);
				}
			}
		}
	}
	SelectOrHoverEntity(myBuilding, hasSelected, hasHovered, line);
}

void PlayerDirector::SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
	, const CU::Intersection::LineSegment3D& aMouseRay)
{
	bool leftClicked;
	if (myRenderGUI == true) // no inworld clicking when mouse is over gui:
	{
		leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0) && !(myGUIManager->MouseOverGUI());
	}
	else
	{
		leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0);
	}

	bool hasPressedShift = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT)
		|| CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RSHIFT);
	bool mouseOnUnit = aEntity->GetComponent<CollisionComponent>()->Collide(aMouseRay);

	if (leftClicked == true && hasPressedShift == false)
	{
		aEntity->SetSelect(false);
	}

	aEntity->SetHovered(false);

	if (mouseOnUnit == true)
	{
		if (leftClicked == true && aSelected == false)
		{
			SelectUnit(aEntity);
			//aEntity->SetSelect(true);
			aSelected = true;
		}
		else if (aHovered == false)
		{
			aEntity->SetHovered(true);
			aHovered = true;
		}
	}
}