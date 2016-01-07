#include "stdafx.h"

#include <Camera.h>
#include "Console.h"
#include <MoveCameraMessage.h>
#include <ColoursForBG.h>
#include "ConsoleState.h"
#include <Engine.h>
#include <Entity.h>
#include <GameStateMessage.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "Level.h"
#include "LevelFactory.h"
#include <MemoryTracker.h>
#include "MessageState.h"
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include "PlayerDirector.h"
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <TimerManager.h>
#include <TriggerMessage.h>
#include <VTuneApi.h>
#include <Vector.h>

#include <ScriptSystem.h>
#include <CinematicMessage.h>
#include <RunScriptMessage.h>

InGameState::InGameState(int aLevelIndex)
	: myShouldReOpenConsole(false)
	, myStartLevelIndex(aLevelIndex)
{
	myIsActiveState = false;
	myIsPlayerCinematic = false;
	myCamera = new Prism::Camera(myCameraOrientation);

	//SetLevel();

	//myCameraOrientation.SetPos(CU::Vector3<float>(10.f, 25.f, 0));
	myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundX(0.0174532925f * 56.f) * myCameraOrientation;

	myCameraOrientation.SetPos(CU::Vector3<float>(30.f, 75.f, 0));
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::MOVE_CAMERA, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TRIGGER, this);
	SAFE_DELETE(myCamera);
	SAFE_DELETE(myLevelFactory);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsShuttingDown = false;
	myIsLetThrough = false;
	myIsComplete = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myCursor = aCursor;
	myLevelFactory = new LevelFactory("Data/Level/LI_level.xml", *myCamera, myCursor);
	myLevel = myLevelFactory->LoadLevel(myStartLevelIndex);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();

	OnResize(windowSize.x, windowSize.y);
	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::MOVE_CAMERA, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::TRIGGER, this);

	PostMaster::GetInstance()->SendMessage(RunScriptMessage("Data/Script/Autorun.script"));

	myIsActiveState = true;
}

void InGameState::EndState()
{
	SAFE_DELETE(myLevel); // need for exit from consol state
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	UpdateCamera(aDeltaTime, myLevel->GetCameraMoveVector());

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) || myIsShuttingDown == true || myIsComplete == true)
	{
		myIsActiveState = false;
		SAFE_DELETE(myLevel);
		return eStateStatus::ePopMainState;
	}

	if (CU::InputWrapper::GetInstance()->KeyUp(DIK_GRAVE) == true || myShouldReOpenConsole == true)
	{
		bool runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
		Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myShouldReOpenConsole = false;
		ConsoleState* newState = new ConsoleState(myShouldReOpenConsole);
		myStateStack->PushSubGameState(newState);
		Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
	}

	if (myIsPlayerCinematic == false)
	{
		if (myLevel->Update(aDeltaTime, *myCamera) == true)
		{
			//return myStateStatus;
		}

		if (myLevel->HasPlayerWon())
		{
			CompleteLevel();
		}
		else if (myLevel->HasAIWon())
		{
			RestartLevel();
		}


		LUA::ScriptSystem::GetInstance()->CallFunction("Update", { aDeltaTime });
	}
	else
	{
		LUA::ScriptSystem::GetInstance()->CallFunction("UpdateCinematic", { { aDeltaTime }, { float(myCinematicIndex) } });
	}



	return myStateStatus;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);
	myLevel->Render(*myCamera);

	if (myIsActiveState == true)
	{
		Prism::DebugDrawer::GetInstance()->Render(*myCamera); //Have to be last
	}

	VTUNE_EVENT_END();
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
}

void InGameState::OnResize(int aWidth, int aHeight)
{
	Prism::ModelLoader::GetInstance()->Pause();
	myLevel->OnResize(aWidth, aHeight);
	Prism::ModelLoader::GetInstance()->UnPause();
}

void InGameState::ReceiveMessage(const GameStateMessage& aMessage)
{
	bool runtime;

	switch (aMessage.myGameState)
	{
	case eGameState::RELOAD_LEVEL:
		runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
		Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myLevel = myLevelFactory->LoadCurrentLevel();
		Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
		PostMaster::GetInstance()->SendMessage(RunScriptMessage("Data/Script/Autorun.script"));
		break;

	case eGameState::COMPLETE_LEVEL:
		if (myLevelFactory->IsLastLevel() == true)
		{
			PostMaster::GetInstance()->SendMessage(OnClickMessage(eOnClickEvent::GAME_WIN));
		}
		else
		{
			CompleteLevel();
		}
		break;

	case eGameState::LOAD_NEXT_LEVEL:
		runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
		Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myLevel = myLevelFactory->LoadNextLevel();
		Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
		break;
	}
}

void InGameState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::GAME_QUIT:
			myIsShuttingDown = true;
			break;
		case eOnClickEvent::GAME_LOSE:
			RestartLevel();
			break;
		case eOnClickEvent::GAME_WIN:
			CompleteGame();
			break;
		case eOnClickEvent::SPAWN_UNIT:
			myLevel->SpawnUnit(static_cast<eUnitType>(aMessage.myID - 1));
			break;
		default:
			break;
		}
	}
}

void InGameState::ReceiveMessage(const MoveCameraMessage& aMessage)
{
	CU::Vector2<float> position;
	if (aMessage.myMovementHandle == eHowToHandleMovement::ZERO_TO_ONE)
	{
		 position = aMessage.myPosition * 255.f;

	}
	else if (aMessage.myMovementHandle == eHowToHandleMovement::WORLD_POSITION)
	{
		position = aMessage.myPosition;
	}
	else
	{
		DL_ASSERT("UNKNOWN HANDLINGTYPE OF CAMERAMOVEMENT");
	}
	float offset = 50.f;

	if (myLevel->GetPlayer()->GetSelectedAction() == eSelectedAction::ATTACK_MOVE)
	{
		myLevel->GetPlayer()->AttackMoveSelectedUnits(position);
	}
	else
	{
		position.y -= offset;
		myCamera->SetPosition({ position.x, myCamera->GetOrientation().GetPos().y, position.y });
		CapCameraToTerrain();
	}
}

void InGameState::ReceiveMessage(const CinematicMessage& aMessage)
{
	if (aMessage.myAction == eCinematicAction::START)
	{
		myIsPlayerCinematic = true;
		myCinematicIndex = aMessage.myCinematicIndex;
	}
	else
	{
		myIsPlayerCinematic = false;
	}
}

void InGameState::ReceiveMessage(const TriggerMessage& aMessage)
{
	LUA::ScriptSystem::GetInstance()->CallFunction("TriggerEvent", { static_cast<float>(aMessage.myTrigger->GetId())
		, static_cast<float>(aMessage.myUnit->GetId()), static_cast<float>(aMessage.myType) });
}

void InGameState::SetLevel()
{
	//myLevel = new Level(*myCamera);
}

void InGameState::RestartLevel()
{
	bool runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	GameStateMessage* newEvent = new GameStateMessage(eGameState::RELOAD_LEVEL);
	ShowMessage("Data/Resource/Texture/Menu/T_background_game_over.dds", { 1024, 1024 }, "Press [space] to restart.", newEvent);
	Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
}

void InGameState::CompleteLevel()
{
	bool runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	GameStateMessage* newEvent = new GameStateMessage(eGameState::LOAD_NEXT_LEVEL);
	ShowMessage("Data/Resource/Texture/Menu/T_background_completed_level.dds", { 1024, 1024 }, "Press [space] to continue.", newEvent);
	Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
}

void InGameState::CompleteGame()
{
	ShowMessage("Data/Resource/Texture/Menu/T_background_completed_game.dds", { 1024, 1024 }, "Press [space] to continue.");
	myIsComplete = true;
}

void InGameState::LoadLevelSettings()
{

}

void InGameState::LoadPlayerSettings()
{

}

void InGameState::UpdateCamera(float aDeltaTime, const CU::Vector3<float>& aCameraMovement)
{
	myCamera->Update(aDeltaTime);

	CU::Vector3<float> cameraPos = myCameraOrientation.GetPos();

	float cameraSpeed = 40.f * aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_DOWNARROW) == true)
	{
		cameraPos.z -= cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_UPARROW) == true)
	{
		cameraPos.z += cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LEFTARROW) == true)
	{
		cameraPos.x -= cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RIGHTARROW) == true)
	{
		cameraPos.x += cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_Q) == true)
	{
		cameraPos.y -= cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_E) == true)
	{
		cameraPos.y += cameraSpeed;
	}

	cameraPos += aCameraMovement * cameraSpeed;

	myCameraOrientation.SetPos(cameraPos);

	float rotationSpeed = 1.f * aDeltaTime;

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_NUMPAD2) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundX(-rotationSpeed) * myCameraOrientation;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_NUMPAD8) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundX(rotationSpeed) * myCameraOrientation;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_NUMPAD4) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundY(-rotationSpeed) * myCameraOrientation;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_NUMPAD6) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundY(rotationSpeed) * myCameraOrientation;
	}

	CapCameraToTerrain();
}

void InGameState::ShowMessage(const std::string& aBackgroundPath,
	const CU::Vector2<float>& aSize, std::string aText, GameStateMessage* aMessage)
{
	bool runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	myIsActiveState = false;
	myMessageScreen = new MessageState(aBackgroundPath, aSize);
	myMessageScreen->SetText(aText);
	myMessageScreen->SetEvent(aMessage);
	myStateStack->PushSubGameState(myMessageScreen);
	Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
}

void InGameState::CapCameraToTerrain()
{
	const CU::Vector2<float>& resolution = Prism::Engine::GetInstance()->GetWindowSize();
	float aspectRatio = resolution.x / resolution.y;

	float offsetX = aspectRatio *(-7.844f) + 58.245f;
	float offsetZLow = aspectRatio*(-12.714f) + 47.602f;
	float offsetZHigh = aspectRatio*(-31.851f) + 86.423f;

	CU::Vector3<float> cameraPos = myCameraOrientation.GetPos();
	if (cameraPos.x < 0.f + offsetX)
	{
		cameraPos.x = 0.f + offsetX;
		myCameraOrientation.SetPos(cameraPos);
	}

	if (cameraPos.z < -50.f + offsetZLow)
	{
		cameraPos.z = -50.f + offsetZLow;
		myCameraOrientation.SetPos(cameraPos);
	}

	if (cameraPos.x > 255.f - offsetX)
	{
		cameraPos.x = 255.f - offsetX;
		myCameraOrientation.SetPos(cameraPos);
	}

	if (cameraPos.z > 205.f - offsetZHigh)
	{
		cameraPos.z = 205.f - offsetZHigh;
		myCameraOrientation.SetPos(cameraPos);
	}
}