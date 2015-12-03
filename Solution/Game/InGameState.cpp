#include "stdafx.h"

#include <Camera.h>
#include <MoveCameraMessage.h>
#include <ColoursForBG.h>
#include <Engine.h>
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
#include <PostMaster.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>

InGameState::InGameState()
{
	myIsActiveState = false;
	myCamera = new Prism::Camera(myCameraOrientation);

	//SetLevel();

	//myCameraOrientation.SetPos(CU::Vector3<float>(10.f, 25.f, 0));
	myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundX(0.0174532925f * 60.f) * myCameraOrientation;

	myCameraOrientation.SetPos(CU::Vector3<float>(30.f, 35.f, 0));
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
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
	myLevel = myLevelFactory->LoadLevel(1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();

	OnResize(windowSize.x, windowSize.y);
	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);


	myIsActiveState = true;
}

void InGameState::EndState()
{

}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	UpdateCamera(aDeltaTime, { 0, 0, 0 });

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) || myIsShuttingDown == true || myIsComplete == true)
	{
		myIsActiveState = false;
		SAFE_DELETE(myLevel);
		return eStateStatus::ePopMainState;
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_M) == true)
	{
		CompleteGame();
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_B) == true)
	{
		RestartLevel();
	}

	if (myLevel->Update(aDeltaTime, *myCamera) == true)
	{
		//return myStateStatus;
	}

	return myStateStatus;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);
	myLevel->Render();

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
		break;

	case eGameState::COMPLETE_LEVEL:

		break;

	case eGameState::LOAD_NEXT_LEVEL:

		break;
	}
}

void InGameState::ReceiveMessage(const OnClickMessage& aMessage)
{
	bool runtime;

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
			runtime = Prism::MemoryTracker::GetInstance()->GetRunTime();
			Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			myLevel->SpawnUnit();
			Prism::MemoryTracker::GetInstance()->SetRunTime(runtime);
			break;
		default:
			break;
		}
	}
}

void InGameState::ReceiveMessage(const MoveCameraMessage& aMessage)
{
	CU::Vector2<float> position = aMessage.myPosition * 255.f;

	myCamera->SetPosition({ position.x, position.y, myCamera->GetOrientation().GetPos().z });
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

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S) == true)
	{
		cameraPos.z -= cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_W) == true)
	{
		cameraPos.z += cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A) == true)
	{
		cameraPos.x -= cameraSpeed;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_D) == true)
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

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_UPARROW) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundX(-rotationSpeed) * myCameraOrientation;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_DOWNARROW) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundX(rotationSpeed) * myCameraOrientation;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LEFTARROW) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundY(-rotationSpeed) * myCameraOrientation;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RIGHTARROW) == true)
	{
		myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundY(rotationSpeed) * myCameraOrientation;
	}
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