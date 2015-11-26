#include "stdafx.h"

#include <Camera.h>
#include <ColoursForBG.h>
#include <Engine.h>
#include <GameStateMessage.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "Level.h"
#include <PostMaster.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>

InGameState::InGameState()
{
	myIsActiveState = false;
	myCamera = new Prism::Camera(myCameraOrientation);

	/*myCameraOrientation.SetPos(CU::Vector3<float>(10.f, 100.f, 0));
	myCameraOrientation = CU::Matrix44<float>::CreateRotateAroundX(0.0174532925f * 70.f) * myCameraOrientation;*/

	myCameraOrientation.SetPos(CU::Vector3<float>(30.f, 80.f, 0));
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	SAFE_DELETE(myCamera);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy)
{
	myIsLetThrough = false;
	myIsComplete = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

	OnResize(windowSize.x, windowSize.y);
	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);



	myIsActiveState = true;
}

void InGameState::EndState()
{
	
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	UpdateCamera(aDeltaTime);
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE))
	{
		myIsActiveState = false;
		delete myLevel;
		myLevel = nullptr;
		return eStateStatus::ePopMainState;
	}
	if (myLevel->LogicUpdate(aDeltaTime) == true)
	{
		return eStateStatus::eKeepState;
	}

	return eStateStatus::eKeepState;
}

void InGameState::Render()
{
	VTUNE_EVENT_BEGIN(VTUNE::GAME_RENDER);
	myLevel->Render();
	Prism::DebugDrawer::GetInstance()->Render(*myCamera); //Have to be last

	VTUNE_EVENT_END();
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
}

void InGameState::OnResize(int aWidth, int aHeight)
{
	myLevel->OnResize(aWidth, aHeight);
}

void InGameState::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.GetGameState())
	{
	case eGameState::RELOAD_LEVEL:
		
		break;

	case eGameState::COMPLETE_LEVEL:
		
		break;

	case eGameState::LOAD_NEXT_LEVEL:
		
		break;
	}
}

void InGameState::SetLevel()
{
	myLevel = new Level(*myCamera);
}

void InGameState::CompleteLevel()
{
	
}

void InGameState::CompleteGame()
{
	
	myIsComplete = true;
}

void InGameState::LoadLevelSettings()
{
	
}

void InGameState::LoadPlayerSettings()
{
	
}

void InGameState::UpdateCamera(float aDeltaTime)
{
	myCamera->Update(aDeltaTime);

	CU::Vector3<float> cameraPos = myCameraOrientation.GetPos();

	float cameraSpeed = 100.f * aDeltaTime;

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
	aBackgroundPath;
	aSize;
	aText;
	aMessage;
}