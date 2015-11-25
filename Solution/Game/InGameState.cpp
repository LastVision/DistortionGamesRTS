#include "stdafx.h"

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
#include <ParticleEmitterData.h>
#include <Camera.h>
#include <ParticleDataContainer.h>

InGameState::InGameState(CU::InputWrapper* anInputWrapper)
{
	myIsActiveState = false;
	myInputWrapper = anInputWrapper;
}

InGameState::~InGameState()
{
	myEmitter->ReleaseData();
	SAFE_DELETE(myEmitter);
	SAFE_DELETE(myCamera);

	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
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

	myEmitter = new Prism::ParticleEmitterInstance();
	myEmitter->Initiate(Prism::ParticleDataContainer::GetInstance()->GetParticleData("Data/Resource/Particle/particle.xml"));
	myEmitter->SetPosition(CU::Vector3f(0, 0, 0));
	myCamera = new Prism::Camera(CU::Matrix44f());
	myIsActiveState = true;
}

void InGameState::EndState()
{
	
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{

	myEmitter->Update(aDeltaTime, CU::Matrix44f());


	if (myInputWrapper->KeyDown(DIK_ESCAPE))
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

	myEmitter->Render(myCamera);
	myLevel->Render(myIsActiveState);

	


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

void InGameState::SetLevel(int aLevelID, int aDifficultID)
{
	myLevel = new Level();
	aLevelID;
	aDifficultID;
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

void InGameState::ShowMessage(const std::string& aBackgroundPath, 
	const CU::Vector2<float>& aSize, std::string aText, GameStateMessage* aMessage)
{
	aBackgroundPath;
	aSize;
	aText;
	aMessage;
}