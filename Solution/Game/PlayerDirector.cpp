#include "stdafx.h"

#include <BuildingComponent.h>
#include <Camera.h>
#include <CollisionComponent.h>
#include <ControllerComponent.h>
#include <TimeMultiplierMessage.h>
#include <EntityFactory.h>
#include <GUIManager.h>
#include <HealthComponent.h>
#include <Intersection.h>
#include <InputWrapper.h>
#include <OnClickMessage.h>
#include <PathFinderAStar.h>
#include "PlayerDirector.h"
#include <PollingStation.h>
#include <Terrain.h>
#include <ToggleGUIMessage.h>
#include <ModelLoader.h>
#include <SpawnUnitMessage.h>
#include <Sprite.h>
#include <FadeMessage.h>
#include <PostMaster.h>

PlayerDirector::PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor)
	: Director(eOwnerType::PLAYER, aTerrain)
	, myRenderGUI(true)
	, myCursor(aCursor)
	, myGUIManager(nullptr)
	, mySelectedUnits(56)
	, myTweakValueX(3.273f)
	, myTweakValueY(10.79f)
	, mySelectedAction(eSelectedAction::NONE)
	, myLeftMouseUp(false)
	, myLeftMouseDown(false)
	, myLeftMousePressed(false)
	, mySelectionSpriteSize(0,0)
	, mySelectionSpriteRenderPosition(0,0)
	, mySelectionSpriteHotspot(0,0)
{
	myDragSelectionPositions.Reserve(4);
	myDragSelectionSprite = new Prism::Sprite("Data/Resource/Texture/T_selection_box.dds", { 0.f, 0.f });

	for (int i = 0; i < 64; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::UNIT, eUnitType::GRUNT, Prism::eOctreeType::DYNAMIC,
			aScene, { 65, 0, 40 }, aTerrain));
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::UNIT, eUnitType::RANGER, Prism::eOctreeType::DYNAMIC,
			aScene, { 65, 0, 40 }, aTerrain));
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::UNIT, eUnitType::TANK, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}
	
	myActiveUnits.Add(myUnits[0]);
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		myActiveUnits[i]->Spawn({ 65.f, 0.f, 25.f });
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits[i]);
	}

	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_GUI, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::TIME_MULTIPLIER, this);
}

PlayerDirector::~PlayerDirector()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myDragSelectionSprite);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_GUI, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TIME_MULTIPLIER, this);
}

void PlayerDirector::InitGUI(const AIDirector* anAI)
{
	Prism::ModelLoader::GetInstance()->Pause();
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_ingame.xml", this, anAI);
	Prism::ModelLoader::GetInstance()->UnPause();
}

void PlayerDirector::Update(float aDeltaTime, const Prism::Camera& aCamera)
{
	aDeltaTime *= myTimeMultiplier;

	//debug only, remove when handled by GUI //Linus
	int playerVictoryPoints = myVictoryPoints;
	DEBUG_PRINT(playerVictoryPoints);
	//debug end

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_G) == true)
	{
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(!myRenderGUI, 1.f / 3.f));
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_H) == true)
	{
		for (int i = 0; i < mySelectedUnits.Size(); i++)
		{
			mySelectedUnits[i]->GetComponent<HealthComponent>()->TakeDamage(1);
		}
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_F2) == true)
	{
		SelectAllUnits();
	}

	UpdateInputs();

	Director::Update(aDeltaTime);
	UpdateMouseInteraction(aCamera);
	myBuilding->Update(aDeltaTime);

	for (int i = mySelectedUnits.Size() - 1; i >= 0; --i) // remove dead units
	{
		if (mySelectedUnits[i]->GetAlive() == false)
		{
			mySelectedUnits[i]->SetSelect(false);
			mySelectedUnits.RemoveCyclicAtIndex(i);
		}
	}

	if (myRenderGUI == true)
	{
		myGUIManager->Update();
	}

	if (myLeftMouseUp == true || myRightClicked == true)
	{
		mySelectedAction = eSelectedAction::NONE;
	}
}

void PlayerDirector::Render(const Prism::Camera& aCamera)
{
	aCamera;
	if (myRenderGUI == true)
	{
		//for (int i = 0; i < mySelectedUnits.Size(); i++)
		//{
		//	mySelectedUnits[i]->GetComponent<HealthComponent>()->RenderHealthBar(aCamera);
		//}
		if (myLeftMousePressed == true)
		{
			myDragSelectionSprite->SetSize(mySelectionSpriteSize, mySelectionSpriteHotspot);
			myDragSelectionSprite->Render(mySelectionSpriteRenderPosition);
		}
		myGUIManager->Render();
	}
}

void PlayerDirector::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

//void PlayerDirector::SpawnUnit(eUnitType aUnitType)
//{
//
//	if (myTestGold >= myBuilding->GetComponent<BuildingComponent>()->GetUnitCost(aUnitType))
//	{
//		myTestGold -= myBuilding->GetComponent<BuildingComponent>()->GetUnitCost(aUnitType);
//		myBuilding->GetComponent<BuildingComponent>()->BuildUnit(aUnitType);
//	}
//}

//void PlayerDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
//{
//	if (aMessage.myOwnerType != static_cast<int>(eOwnerType::PLAYER)) return;
//	if (myActiveUnits.Size() < 64)
//	{
//		for (int i = 0; i < myUnits.Size(); ++i)
//		{
//			if (myUnits[i]->GetUnitType() == static_cast<eUnitType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
//			{
//				if (IsAlreadyActive(myUnits[i]) == true)
//				{
//					continue;
//				}
//
//				myUnits[i]->Spawn(myBuilding->GetOrientation().GetPos() + CU::Vector3f(0.f, 0.f, -15.f));
//				myActiveUnits.Add(myUnits[i]);
//				break;
//
//			}
//		}
//		PollingStation::GetInstance()->RegisterEntity(myActiveUnits.GetLast());
//	}
//}

void PlayerDirector::ReceiveMessage(const ToggleGUIMessage& aMessage)
{
	myRenderGUI = aMessage.myShowGUI;
	PostMaster::GetInstance()->SendMessage(FadeMessage(aMessage.myFadeTime));
}

void PlayerDirector::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (mySelectedUnits.Size() > 0)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::UNIT_ACTION_ATTACK:
			mySelectedAction = eSelectedAction::ATTACK_TAGRET;
			break;

		case eOnClickEvent::UNIT_ACTION_ATTACK_MOVE:
			mySelectedAction = eSelectedAction::ATTACK_MOVE;
			break;

		case eOnClickEvent::UNIT_ACTION_MOVE:
			mySelectedAction = eSelectedAction::MOVE;
			break;

		case eOnClickEvent::UNIT_ACTION_PATROL:
			mySelectedAction = eSelectedAction::PATROL;
			break;

		case eOnClickEvent::UNIT_ACTION_STAND_GROUND:
			mySelectedAction = eSelectedAction::HOLD_POSITION;
			break;

		case eOnClickEvent::UNIT_ACTION_STOP:
			mySelectedAction = eSelectedAction::STOP;
			break;
		
		default:
			break;
		}
	}
}

void PlayerDirector::ReceiveMessage(const TimeMultiplierMessage& aMessage)
{
	if (aMessage.myOwner == eOwnerType::PLAYER)
	{
		myTimeMultiplier = aMessage.myMultiplier;
	}
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

CU::Vector3<float> PlayerDirector::CalcCursorWorldPosition(const CU::Vector2<float>& aMousePosition, const Prism::Camera& aCamera)
{
	CU::Vector2<float> inputPos = aMousePosition;
	CU::Vector2<float> cursorPos;
	CU::Vector2<float> window = Prism::Engine::GetInstance()->GetWindowSize();
	//myTweakValueX = 3.284f;
	//myTweakValueY = 3.5f;
	float epsilon = 0.1f;
	float aspect = window.x / window.y;
	if (aspect <= 5.f / 4.f + epsilon)
	{
		myTweakValueX = 4.667f;
		myTweakValueY = 7.313f;
	}
	else if (aspect <= 16.f / 10.f + epsilon)
	{
		//tweakValue = 1.605f; // FOV90
		myTweakValueX = 3.646f;
		myTweakValueY = 9.315f;
	}

	float mult = window.y / window.x;
	cursorPos = inputPos;


	cursorPos.y = window.y - cursorPos.y;
	cursorPos.y /= window.y;
	cursorPos.y *= mult / myTweakValueY;
	cursorPos.y += (1.f - mult / myTweakValueY) / 2.f;


	cursorPos.x /= window.x;
	cursorPos.x *= mult / myTweakValueX;
	cursorPos.x += (1.f - mult / myTweakValueX) / 2.f;

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_L) == true)
	{
		myTweakValueX += 0.001f;
		myTweakValueY += 0.001f;
		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) == true)
		{
			myTweakValueX += 0.1f;
			myTweakValueY += 0.1f;
		}
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_K) == true)
	{
		myTweakValueX -= 0.001f;
		myTweakValueY -= 0.001f;
		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) == true)
		{
			myTweakValueX -= 0.1f;
			myTweakValueY -= 0.1f;
		}
	}

	DEBUG_PRINT(myTweakValueX);
	DEBUG_PRINT(myTweakValueY);
	DEBUG_PRINT(cursorPos);
	

	CU::Vector3<float> worldPos(myTerrain.CalcIntersection(aCamera.GetOrientation().GetPos()
		, aCamera.RayCast(cursorPos)));

	DEBUG_PRINT(worldPos);

	//Debug:
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });

	return worldPos;
}

void PlayerDirector::UpdateInputs()
{
	myShiftPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT)
		|| CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RSHIFT);

	if (mySelectedUnits.Size() > 0)
	{
		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A) == true)
		{
			mySelectedAction = eSelectedAction::ATTACK_MOVE;
		}

		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S) == true)
		{
			mySelectedAction = eSelectedAction::STOP;
		}

		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_M) == true)
		{
			mySelectedAction = eSelectedAction::MOVE;
		}

		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_H) == true)
		{
			mySelectedAction = eSelectedAction::HOLD_POSITION;
		}
	}

	if (myRenderGUI == true) // no inworld clicking when mouse is over gui:
	{
		myLeftMouseDown = CU::InputWrapper::GetInstance()->MouseDown(0) && !(myGUIManager->MouseOverGUI());
		myLeftMousePressed = CU::InputWrapper::GetInstance()->MouseIsPressed(0) && !(myGUIManager->MouseOverGUI());

		if (myLeftMouseDown == true)
		{
			myFirstMousePosition = CU::InputWrapper::GetInstance()->GetMousePosition();
	}

		myLeftMouseUp = CU::InputWrapper::GetInstance()->MouseUp(0) && !(myGUIManager->MouseOverGUI());
		myRightClicked = CU::InputWrapper::GetInstance()->MouseUp(1) && !(myGUIManager->MouseOverGUI());
	}
	else
	{
		myLeftMouseDown = CU::InputWrapper::GetInstance()->MouseDown(0);

		if (myLeftMouseDown == true)
		{
			myFirstMousePosition = CU::InputWrapper::GetInstance()->GetMousePosition();
	}

		myLeftMouseUp = CU::InputWrapper::GetInstance()->MouseUp(0);
		myRightClicked = CU::InputWrapper::GetInstance()->MouseUp(1);
	}

	if (myLeftMouseUp == true && myShiftPressed == false &&
		(mySelectedAction == eSelectedAction::NONE || mySelectedAction == eSelectedAction::STOP 
		|| mySelectedAction == eSelectedAction::HOLD_POSITION))
	{
		mySelectedUnits.RemoveAll();
	}
}

void PlayerDirector::UpdateMouseInteraction(const Prism::Camera& aCamera)
{
	CU::Vector3<float> firstTargetPos = CalcCursorWorldPosition(CU::InputWrapper::GetInstance()->GetMousePosition(), aCamera);
	CU::Vector3<float> secondTargetPos;
	CU::Vector2<float> mousePosition = CU::InputWrapper::GetInstance()->GetMousePosition();

	if (myLeftMousePressed == true)
	{
		secondTargetPos = CalcCursorWorldPosition(myFirstMousePosition, aCamera);

		myDragSelectionPositions[0] = { fminf(secondTargetPos.x, firstTargetPos.x), secondTargetPos.y, fminf(secondTargetPos.z, firstTargetPos.z) };
		myDragSelectionPositions[1] = { fminf(secondTargetPos.x, firstTargetPos.x), secondTargetPos.y, fmaxf(secondTargetPos.z, firstTargetPos.z) };
		myDragSelectionPositions[2] = { fmaxf(secondTargetPos.x, firstTargetPos.x), secondTargetPos.y, fmaxf(secondTargetPos.z, firstTargetPos.z) };
		myDragSelectionPositions[3] = { fmaxf(secondTargetPos.x, firstTargetPos.x), secondTargetPos.y, fminf(secondTargetPos.z, firstTargetPos.z) };

		mySelectionSpriteSize = { fabs(mousePosition.x - myFirstMousePosition.x), fabs(mousePosition.y - myFirstMousePosition.y) };
		mySelectionSpriteHotspot = { 0.f, fabs(mousePosition.y - myFirstMousePosition.y) };

		CU::Vector2<float> renderPosition;
		renderPosition.x = fminf(myFirstMousePosition.x, mousePosition.x);
		renderPosition.y = Prism::Engine::GetInstance()->GetWindowSize().y;
		renderPosition.y -= fminf(myFirstMousePosition.y, mousePosition.y);
		mySelectionSpriteRenderPosition = renderPosition;


		//Prism::RenderLine3D(myDragSelectionPositions[0], myDragSelectionPositions[1]);
		//Prism::RenderLine3D(myDragSelectionPositions[1], myDragSelectionPositions[2]);
		//Prism::RenderLine3D(myDragSelectionPositions[2], myDragSelectionPositions[3]);
		//Prism::RenderLine3D(myDragSelectionPositions[3], myDragSelectionPositions[0]);
	}

	Entity* hoveredEnemy = PollingStation::GetInstance()->FindEntityAtPosition(firstTargetPos, eOwnerType::ENEMY);
	if (hoveredEnemy != nullptr)
	{
		Prism::RenderBox(hoveredEnemy->GetOrientation().GetPos(), eColorDebug::RED);
	}

	bool hasSelected = false;
	bool hasHovered = false;
	bool hasDoneAction = false;

	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), firstTargetPos);

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		SelectOrHoverEntity(myUnits[i], hasSelected, hasHovered, line);
		ControllerComponent* controller = myUnits[i]->GetComponent<ControllerComponent>();

		if (myUnits[i]->IsSelected())
		{
			if ((mySelectedAction == eSelectedAction::ATTACK_TAGRET && hoveredEnemy != nullptr && myLeftMouseUp == true) 
				|| (hoveredEnemy != nullptr && myRightClicked == true))
			{
				controller->AttackTarget(hoveredEnemy, !myShiftPressed);
				hasDoneAction = true;
			}
			else if (mySelectedAction == eSelectedAction::ATTACK_MOVE && myLeftMouseUp == true)
			{
				controller->AttackMove(firstTargetPos, !myShiftPressed);
				hasDoneAction = true;
			}
			else if ((mySelectedAction == eSelectedAction::MOVE && myLeftMouseUp) || myRightClicked)
			{
				controller->MoveTo(firstTargetPos, !myShiftPressed);
				hasDoneAction = true;
			}
			else if (mySelectedAction == eSelectedAction::STOP)
			{
				controller->Stop();
				hasDoneAction = true;
			}
			else if (mySelectedAction == eSelectedAction::HOLD_POSITION)
			{
				controller->HoldPosition();
				hasDoneAction = true;
			}
		}
	}

	if (hasDoneAction == true)
	{
		mySelectedAction = eSelectedAction::NONE;
	}

	SelectOrHoverEntity(myBuilding, hasSelected, hasHovered, line);
}

void PlayerDirector::SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
	, const CU::Intersection::LineSegment3D& aMouseRay)
{
	aSelected;
	aMouseRay;
	if (myLeftMouseDown == true && myShiftPressed == false
		&& (mySelectedAction == eSelectedAction::NONE || mySelectedAction == eSelectedAction::HOLD_POSITION
		|| mySelectedAction == eSelectedAction::STOP))
	{
		aEntity->SetSelect(false);
	}

	aEntity->SetHovered(false);

	CU::Vector2<float> position1(myDragSelectionPositions[0].x, myDragSelectionPositions[0].z);
	CU::Vector2<float> position2(myDragSelectionPositions[2].x, myDragSelectionPositions[2].z);


	if (aEntity->GetComponent<CollisionComponent>()->Collide(position1, position2) == true)
	{
		if (myLeftMouseUp == true)
		{
			SelectUnit(aEntity);
			//aSelected = true;
		}
		else if (aHovered == false)
		{
			aEntity->SetHovered(true);
			aHovered = true;
		}
	}
}

void PlayerDirector::SelectAllUnits()
{
	mySelectedUnits.RemoveAll();
	myBuilding->SetSelect(false);
	myBuilding->SetHovered(false);

	for (int i = 0; i < myUnits.Size(); i++)
	{
		if (myUnits[i]->GetAlive() == true)
		{
			SelectUnit(myUnits[i]);
		}
	}
}