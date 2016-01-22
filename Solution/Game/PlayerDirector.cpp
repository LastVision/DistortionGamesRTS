#include "stdafx.h"

#include <AudioInterface.h>
#include <BuildingComponent.h>
#include <Camera.h>
#include <CollisionComponent.h>
#include <ControllerComponent.h>
#include <TimeMultiplierMessage.h>
#include <EnrageComponent.h>
#include <Entity.h>
#include <EntityData.h>
#include <EntityFactory.h>
#include <EventPositionMessage.h>
#include <GUIManager.h>
#include <GraphicsComponent.h>
#include <HealthComponent.h>
#include <Intersection.h>
#include <InputWrapper.h>
#include <MinimapMoveMessage.h>
#include <MoveCameraMessage.h>
#include <OnClickMessage.h>
#include "PlayerDirector.h"
#include <PollingStation.h>
#include <Terrain.h>
#include <ToggleGUIMessage.h>
#include <ToggleBuildTimeMessage.h>
#include <TotemComponent.h>
#include <ModelLoader.h>
#include <SpawnUnitMessage.h>
#include <SpriteProxy.h>
#include <FadeMessage.h>
#include <PostMaster.h>
#include <XMLReader.h>

PlayerDirector::PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor)
	: Director(eOwnerType::PLAYER, aTerrain)
	, myRenderGUI(true)
	, myCursor(aCursor)
	, myGUIManager(nullptr)
	, mySelectedUnits(56)
	, myTweakValueX(3.273f)
	, myTweakValueY(10.79f)
	, mySelectedAction(eSelectedAction::NONE)
	, myControlPressed(false)
	, myLeftMouseUp(false)
	, myLeftMouseDown(false)
	, myMouseIsOverGUI(false)
	, myLeftMousePressed(false)
	, myRenderDragSelection(true)
	, mySelectionSpriteSize(0, 0)
	, mySelectionSpriteRenderPosition(0, 0)
	, mySelectionSpriteHotspot(0, 0)
	, myAudioSFXID(-1)
	, myMaxSelectedUnits(0)
	, myHasEventToGoTo(false)
{
	myAudioSFXID = Prism::Audio::AudioInterface::GetInstance()->GetUniqueID();
	myDragSelectionPositions.Reserve(4);
	myDragSelectionSpriteVertical = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_selection_box_vertical.dds", { 0.f, 0.f });
	myDragSelectionSpriteHorizontal = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_selection_box_horizontal.dds", { 0.f, 0.f });

	for (int i = 0; i < 64; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::UNIT, "grunt", Prism::eOctreeType::DYNAMIC,
			aScene, { 65, 0, 40 }, aTerrain));
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::UNIT, "ranger", Prism::eOctreeType::DYNAMIC,
			aScene, { 65, 0, 40 }, aTerrain));
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::UNIT, "tank", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_GUI, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::TIME_MULTIPLIER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::MOVE_UNITS, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_BUILD_TIME, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::EVENT_POSITION, this);

	EntityData tempData;

	XMLReader reader;
	reader.OpenDocument("Data/Resource/Entity/E_totem.xml");
	tinyxml2::XMLElement* totemElement = reader.ForceFindFirstChild("root");
	totemElement = reader.ForceFindFirstChild(totemElement, "Totem");

	if (totemElement != nullptr)
	{
		std::string modelPath;
		std::string effectPath;
		tempData.myGraphicsData.myExistsInEntity = true;
		tempData.myTotemData.myExistsInEntity = true;

		tinyxml2::XMLElement* e = reader.ForceFindFirstChild(totemElement, "Model");
		reader.ForceReadAttribute(e, "modelPath", modelPath);
		tempData.myGraphicsData.myModelPath = modelPath.c_str();
		reader.ForceReadAttribute(e, "shaderPath", effectPath);
		tempData.myGraphicsData.myEffectPath = effectPath.c_str();

		float valueToUse = 0.f;

		e = reader.ForceFindFirstChild(totemElement, "HealPerSecond");
		reader.ForceReadAttribute(e, "value", valueToUse);
		tempData.myTotemData.myHealPerSecond = valueToUse;

		e = reader.ForceFindFirstChild(totemElement, "Radius");
		reader.ForceReadAttribute(e, "value", valueToUse);
		tempData.myTotemData.myRadius = valueToUse;

		e = reader.ForceFindFirstChild(totemElement, "Cooldown");
		reader.ForceReadAttribute(e, "value", valueToUse);
		tempData.myTotemData.myCooldown = valueToUse;
		
		e = reader.ForceFindFirstChild(totemElement, "Duration");
		reader.ForceReadAttribute(e, "value", valueToUse);
		tempData.myTotemData.myDuration = valueToUse;

		reader.CloseDocument();
	}


	tempData.mySoundData.myExistsInEntity = true;


	myTotem = new Entity(eOwnerType::PLAYER, Prism::eOctreeType::DYNAMIC, tempData, aScene, { 128.f, 100.f, 128.f },
		aTerrain, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }, eUnitType::NOT_A_UNIT);
	myTotem->AddToScene();

	reader.OpenDocument("Data/Setting/SET_game.xml");
	tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "maxSelected"), "value", myMaxSelectedUnits);
	reader.CloseDocument();
}

PlayerDirector::~PlayerDirector()
{
	//myTotem->RemoveFromScene();
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myDragSelectionSpriteVertical);
	SAFE_DELETE(myDragSelectionSpriteHorizontal);
	SAFE_DELETE(myTotem);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_GUI, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TIME_MULTIPLIER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::MOVE_UNITS, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_BUILD_TIME, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::EVENT_POSITION, this);
	Prism::Audio::AudioInterface::GetInstance()->UnRegisterObject(myAudioSFXID);
}

void PlayerDirector::InitGUI(const AIDirector* anAI, const Prism::Camera& aCamera, int aLeveID)
{
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_ingame.xml", this, anAI, &aCamera, aLeveID);

}

void PlayerDirector::Update(float aDeltaTime, const Prism::Camera& aCamera)
{
	aDeltaTime *= myTimeMultiplier;
	DEBUG_PRINT(myHasUnlockedRanger);
	DEBUG_PRINT(myHasUnlockedTank);
	//Prism::Audio::AudioInterface::GetInstance()->SetListenerPosition(aCamera.GetOrientation().GetPos().x
	//	, aCamera.GetOrientation().GetPos().y, aCamera.GetOrientation().GetPos().z);
	Prism::Audio::AudioInterface::GetInstance()->SetListenerPosition(aCamera.GetOrientation().GetPos().x
		, 7.5f, aCamera.GetOrientation().GetPos().z + 25.f);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_G) == true)
	{
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(!myRenderGUI, 1.f / 3.f));
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_F2) == true)
	{
		SelectAllUnits();
	}

	UpdateInputs();

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE))
	{
		if (myHasEventToGoTo == true)
		{
			PostMaster::GetInstance()->SendMessage(MoveCameraMessage(myLastEventPosition, eHowToHandleMovement::WORLD_POSITION));
			myHasEventToGoTo = false;
		}
	}

	Director::Update(aDeltaTime);
	UpdateMouseInteraction(aCamera);
	myBuilding->Update(aDeltaTime);
	myTotem->Update(aDeltaTime);

	for (int i = mySelectedUnits.Size() - 1; i >= 0; --i) // remove dead units
	{
		if (mySelectedUnits[i]->GetAlive() == false)
		{
			mySelectedUnits[i]->SetSelect(false);
			mySelectedUnits.RemoveCyclicAtIndex(i);
		}
	}

	UpdateControlGroups();

	if (myRenderGUI == true)
	{
		myGUIManager->Update(aDeltaTime);
	}

	if (myMouseIsOverGUI == false && (myLeftMouseUp == true || myRightClicked == true))
	{
		mySelectedAction = eSelectedAction::NONE;
	}
}

void PlayerDirector::Render(const Prism::Camera& aCamera)
{
	aCamera;
	if (myRenderGUI == true)
	{
		if (myLeftMousePressed == true && myRenderDragSelection == true) // balck magic numbers, don't change
		{
			myDragSelectionSpriteVertical->SetSize({ 10.f, mySelectionSpriteSize.y }, mySelectionSpriteHotspot);
			myDragSelectionSpriteVertical->Render(mySelectionSpriteRenderPosition);

			myDragSelectionSpriteHorizontal->SetSize({ mySelectionSpriteSize.x, 10.f }, mySelectionSpriteHotspot);
			myDragSelectionSpriteHorizontal->Render(mySelectionSpriteRenderPosition);

			myDragSelectionSpriteVertical->SetSize({ 10.f, mySelectionSpriteSize.y }, { -mySelectionSpriteSize.x, mySelectionSpriteSize.y });
			myDragSelectionSpriteVertical->Render(mySelectionSpriteRenderPosition);

			myDragSelectionSpriteHorizontal->SetSize({ mySelectionSpriteSize.x + 10.f, 10.f }, { mySelectionSpriteHotspot.x, mySelectionSpriteHotspot.y - mySelectionSpriteSize.y });
			myDragSelectionSpriteHorizontal->Render(mySelectionSpriteRenderPosition);
		}
		myGUIManager->Render();
	}
}

void PlayerDirector::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void PlayerDirector::ReceiveMessage(const ToggleGUIMessage& aMessage)
{
	myRenderGUI = aMessage.myShowGUI;
	PostMaster::GetInstance()->SendMessage(FadeMessage(aMessage.myFadeTime));
}

void PlayerDirector::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (aMessage.myEvent == eOnClickEvent::PLACE_TOTEM)
	{
		mySelectedAction = eSelectedAction::PLACE_TOTEM;
		return;
	}
	else if (aMessage.myEvent == eOnClickEvent::UPGRADE_UNIT)
	{
		UpgradeUnit(static_cast<eUnitType>(aMessage.myID));
		return;
	}

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

		case eOnClickEvent::UNIT_ACTION_STAND_GROUND:
			mySelectedAction = eSelectedAction::HOLD_POSITION;
			break;

		case eOnClickEvent::UNIT_ACTION_STOP:
			mySelectedAction = eSelectedAction::STOP;
			break;

		case eOnClickEvent::ENRAGE:
			mySelectedAction = eSelectedAction::ENRAGE;
			break;

		default:
			break;
		}
	}
	else if (aMessage.myEvent == eOnClickEvent::SELECT_CONTROL_GROUP)
	{
		if (myControlGroups[aMessage.myID].Size() > 0)
		{
			for (int i = 0; i < mySelectedUnits.Size(); i++)
			{
				mySelectedUnits[i]->SetSelect(false);
			}
			mySelectedUnits = myControlGroups[aMessage.myID];
			for (int i = 0; i < mySelectedUnits.Size(); i++)
			{
				mySelectedUnits[i]->SetSelect(true);
			}
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

void PlayerDirector::ReceiveMessage(const MinimapMoveMessage& aMessage)
{
	CU::Vector2<float> position = aMessage.myPosition * 255.f;
	bool myHasPlayedSound = false;
	for (int i = 0; i < mySelectedUnits.Size(); i++)
	{
		mySelectedUnits[i]->GetComponent<ControllerComponent>()->MoveTo({ position.x, 0.f, position.y }, true, myHasPlayedSound);
	}
}

void PlayerDirector::ReceiveMessage(const ToggleBuildTimeMessage& aMessage)
{
	myBuilding->GetComponent<BuildingComponent>()->SetIgnoreBuildTime(aMessage.myIgnoreBuildTime);
}

void PlayerDirector::ReceiveMessage(const EventPositionMessage& aMessage)
{
	myLastEventPosition = aMessage.myPosition;
	myHasEventToGoTo = true;
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
	if (mySelectedUnits.Size() >= myMaxSelectedUnits) // max selected
	{
		anEntity->SetSelect(false);
		anEntity->SetHovered(false);
		return;
	}

	if (mySelectedUnits.Size() > 0 && mySelectedUnits[0]->GetType() != anEntity->GetType())
	{
		if (mySelectedUnits[0]->GetType() == eEntityType::BASE_BUILING)
		{
			myBuilding->SetSelect(false);
			myBuilding->SetHovered(false);
			mySelectedUnits.RemoveAll();
		}
		else
		{
			return;
		}
	}

	bool shouldAddToSelectedUnits = true;

	for (int i = 0; i < mySelectedUnits.Size(); i++)
	{
		if (mySelectedUnits[i] == anEntity)
		{
			if (anEntity->IsSelected() == true)
			{
				return;
			}
	
			shouldAddToSelectedUnits = false;
		}
	}

	anEntity->SetSelect(true);
	if (shouldAddToSelectedUnits == true)
	{
		mySelectedUnits.Add(anEntity);
	}
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
#ifndef RELEASE_BUILD
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });
#endif

	return worldPos;
}
	
const float& PlayerDirector::GetTotemCooldown() const
{
	return myTotem->GetComponent<TotemComponent>()->GetCurrentCooldown();
}

const int& PlayerDirector::GetUpgradeLevel(int aUnitID) const
{
	return myBuilding->GetComponent<BuildingComponent>()->GetUpgradeLevel(aUnitID);
}

const float& PlayerDirector::GetUpgradeCooldown(int aUnitID) const
{
	return myBuilding->GetComponent<BuildingComponent>()->GetUpgradeCooldown(aUnitID);
}

void PlayerDirector::UpdateInputs()
{
	myShiftPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT)
		|| CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RSHIFT);
	myControlPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LCONTROL)
		|| CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RCONTROL);
	myMouseIsOverGUI = myGUIManager->MouseOverGUI();

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

	myLeftMouseDown = CU::InputWrapper::GetInstance()->MouseDown(0);
	myLeftMousePressed = CU::InputWrapper::GetInstance()->MouseIsPressed(0);
	myLeftMouseUp = CU::InputWrapper::GetInstance()->MouseUp(0);
	myRightClicked = CU::InputWrapper::GetInstance()->MouseDown(1);

	if (myMouseIsOverGUI == false && myLeftMouseUp == true && myShiftPressed == false &&
		(mySelectedAction == eSelectedAction::NONE || mySelectedAction == eSelectedAction::STOP
		|| mySelectedAction == eSelectedAction::HOLD_POSITION))
	{
		mySelectedUnits.RemoveAll();
	}
}

void PlayerDirector::UpdateControlGroups()
{
	if (mySelectedUnits.Size() > 0 && myControlPressed == true)
	{
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1) == true)
		{
			myControlGroups[0] = mySelectedUnits;
		}
		else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_2) == true)
		{
			myControlGroups[1] = mySelectedUnits;
		}
		else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_3) == true)
		{
			myControlGroups[2] = mySelectedUnits;
		}
		else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4) == true)
		{
			myControlGroups[3] = mySelectedUnits;
		}
	}

	for (int i = 0; i < AMOUNT_OF_CONTROL_GROUPS; i++)
	{
		for (int j = myControlGroups[i].Size() - 1; j >= 0; --j)
		{
			if (myControlGroups[i][j]->GetAlive() == false)
			{
				myControlGroups[i].RemoveCyclicAtIndex(j);
			}
		}
	}

	if (myControlPressed == false)
	{
		int index = -1;

		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1) == true)
		{
			index = 0;
		}
		else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_2) == true)
		{
			index = 1;
		}
		else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_3) == true)
		{
			index = 2;
		}
		else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4) == true)
		{
			index = 3;
		}

		if (index != -1 && myControlGroups[index].Size() > 0)
		{
			for (int i = 0; i < mySelectedUnits.Size(); i++)
			{
				mySelectedUnits[i]->SetSelect(false);
			}
			mySelectedUnits = myControlGroups[index];
			for (int i = 0; i < mySelectedUnits.Size(); i++)
			{
				mySelectedUnits[i]->SetSelect(true);
			}
		}
	}
}

void PlayerDirector::UpdateMouseInteraction(const Prism::Camera& aCamera)
{
	CU::Vector3<float> firstTargetPos = CalcCursorWorldPosition(CU::InputWrapper::GetInstance()->GetMousePosition(), aCamera);
	CU::Vector3<float> secondTargetPos;
	CU::Vector2<float> mousePosition = CU::InputWrapper::GetInstance()->GetMousePosition();

	if ((myLeftMouseDown == true || myRightClicked == true) && myMouseIsOverGUI == true)
	{
		myRenderDragSelection = false;
	}

	if (myLeftMouseDown == true && mySelectedAction == eSelectedAction::PLACE_TOTEM)
	{
		PlaceTotem(firstTargetPos);
	}

	if (mySelectedAction == eSelectedAction::ENRAGE)
	{
		Enrage();
	}

	if (myLeftMouseDown == true)
	{
		myFirstMousePosition = CU::InputWrapper::GetInstance()->GetMousePosition();
		myFirstMousePositionInWorld = CalcCursorWorldPosition(myFirstMousePosition, aCamera);
		myFirstCameraPosition = aCamera.GetOrientation().GetPos();

		if (myMouseIsOverGUI == true)
		{
			myRenderDragSelection = false;
		}
		else
		{
			myRenderDragSelection = true;
		}
	}

	if (myLeftMousePressed == true && myRenderDragSelection == true)
	{
		secondTargetPos = myFirstMousePositionInWorld;

		if (myFirstCameraPosition != aCamera.GetOrientation().GetPos())
		{
			CU::Vector3<float> difference = (aCamera.GetOrientation().GetPos() - myFirstCameraPosition) * 40.f;
			CU::Vector2<float> screenPosition = { difference.x, difference.z };
			myFirstMousePosition.x -= screenPosition.x;
			myFirstMousePosition.y += screenPosition.y;

			myFirstCameraPosition = aCamera.GetOrientation().GetPos();
		}

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
	}

	Entity* hoveredEnemy = PollingStation::GetInstance()->FindEntityAtPosition(firstTargetPos, eOwnerType::ENEMY | eOwnerType::NEUTRAL);
	if (hoveredEnemy != nullptr)
	{
		Prism::RenderBox(hoveredEnemy->GetOrientation().GetPos(), eColorDebug::RED);
	}

	bool hasSelected = false;
	bool hasHovered = false;
	bool hasDoneAction = false;

	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), firstTargetPos);
	bool myHasPlayedSound = false;
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		SelectOrHoverEntity(myActiveUnits[i], hasSelected, hasHovered, line);

		if (myMouseIsOverGUI == false && myActiveUnits[i]->IsSelected())
		{
			ControllerComponent* controller = myActiveUnits[i]->GetComponent<ControllerComponent>();
			if ((mySelectedAction == eSelectedAction::ATTACK_TAGRET && hoveredEnemy != nullptr && myLeftMouseUp == true)
				|| (hoveredEnemy != nullptr && myRightClicked == true))
			{
				controller->AttackTarget(hoveredEnemy, !myShiftPressed, myHasPlayedSound);
				hasDoneAction = true;
			}
			else if (mySelectedAction == eSelectedAction::ATTACK_MOVE && myLeftMouseUp == true)
			{
				controller->AttackMove(firstTargetPos, !myShiftPressed, myHasPlayedSound);
				hasDoneAction = true;
			}
			else if ((mySelectedAction == eSelectedAction::MOVE && myLeftMouseUp) || myRightClicked)
			{
				controller->MoveTo(firstTargetPos, !myShiftPressed, myHasPlayedSound);
				hasDoneAction = true;
			}
			else if (mySelectedAction == eSelectedAction::STOP)
			{
				controller->Stop(myHasPlayedSound);
				hasDoneAction = true;
			}
			else if (mySelectedAction == eSelectedAction::HOLD_POSITION)
			{
				controller->HoldPosition(myHasPlayedSound);
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

	if (myLeftMouseDown == true && myShiftPressed == false && myMouseIsOverGUI == false
		&& (mySelectedAction == eSelectedAction::NONE || mySelectedAction == eSelectedAction::HOLD_POSITION
		|| mySelectedAction == eSelectedAction::STOP))
	{
		aEntity->SetSelect(false);
	}
	aEntity->SetHovered(false);

	bool unitCollided = false;
	if (myRenderDragSelection == true)
	{
		CU::Vector2<float> position1(myDragSelectionPositions[0].x, myDragSelectionPositions[0].z);
		CU::Vector2<float> position2(myDragSelectionPositions[2].x, myDragSelectionPositions[2].z);

		if (aEntity->GetComponent<CollisionComponent>()->Collide(position1, position2) == true)
		{
			unitCollided = true;
		}
		else
		{
			if (aEntity->GetComponent<CollisionComponent>()->Collide(aMouseRay) == true)
			{
				unitCollided = true;
			}
		}
	}
	else
	{
		if (aEntity->GetComponent<CollisionComponent>()->Collide(aMouseRay) == true)
		{
			unitCollided = true;
		}
	}

	if (unitCollided == true)
	{
		if (myLeftMouseUp == true)
		{
			SelectUnit(aEntity);
		}
		else
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

void PlayerDirector::PlaceTotem(const CU::Vector3f& aPositionInWorld)
{
	//myTotem->SetPosition(aPositionInWorld);
	myTotem->GetComponent<TotemComponent>()->SetTargetPosition(aPositionInWorld);
}

void PlayerDirector::Enrage()
{
	for (int i = 0; i < mySelectedUnits.Size(); ++i)
	{
		EnrageComponent* enrageComp = mySelectedUnits[i]->GetComponent<EnrageComponent>();
		if (enrageComp != nullptr)
		{
			enrageComp->Activate();
		}
	}
}

void PlayerDirector::AttackMoveSelectedUnits(const CU::Vector2<float>& aPosition)
{
	bool myHasPlayedSound = false;
	for (int i = 0; i < mySelectedUnits.Size(); i++)
	{
		mySelectedUnits[i]->GetComponent<ControllerComponent>()->AttackMove({ aPosition.x, 0.f, aPosition.y }, !myShiftPressed, myHasPlayedSound);
	}
	mySelectedAction = eSelectedAction::NONE;
}