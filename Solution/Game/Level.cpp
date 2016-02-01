#include "stdafx.h"

#include "AIDirector.h"
#include <Camera.h>
#include <DirectionalLight.h>
#include "EmitterManager.h"
#include <Engine.h>
#include <EngineEnums.h>
#include <Entity.h>
#include <EntityId.h>
#include <EntityFactory.h>
#include <FogOfWarHelper.h>
#include "FogOfWarMap.h"
#include <GameStateMessage.h>
#include <InputWrapper.h>
#include "Level.h"
#include "NeutralDirector.h"
#include "PlayerDirector.h"
#include "PollingStation.h"
#include <PostMaster.h>
#include <Renderer.h>
#include <Scene.h>
#include <ScriptSystem.h>
#include <Terrain.h>
#include <ToggleRenderLinesMessage.h>
#include "Tutorial.h"

Level::Level(const Prism::Camera& aCamera, Prism::Terrain* aTerrain, GUI::Cursor* aCursor)
	: myEntities(64)
	, myRenderNavMeshLines(false)
	, myShowFogOfWar(true)
	, myMaxVictoryPoint(-1)
{
	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_entity.xml");
	myEmitterManager = new EmitterManager(aCamera);
	myTerrain = aTerrain;

	myScene = new Prism::Scene(aCamera, *myTerrain);

	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_RENDER_LINES, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_FOG_OF_WAR, this);

	myPlayer = new PlayerDirector(*myTerrain, *myScene, aCursor);
	myAI = new AIDirector(*myTerrain, *myScene);

	myNeutralDirector = new NeutralDirector(*myTerrain, *myScene);

	myRenderer = new Prism::Renderer();

	myFogOfWarHelper = new Prism::FogOfWarHelper();
	FogOfWarMap::GetInstance();
	FogOfWarMap::GetInstance()->SetFogOfWarHelperTexture(myFogOfWarHelper->GetTexture());
}

Level::~Level()
{
	myEntities.DeleteAll();
	SAFE_DELETE(myTutorial);
	SAFE_DELETE(myEmitterManager);
	SAFE_DELETE(myTerrain);
	SAFE_DELETE(myLight);
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myAI);
	SAFE_DELETE(myNeutralDirector);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myRenderer);
	FogOfWarMap::Destroy();
	SAFE_DELETE(myFogOfWarHelper);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_RENDER_LINES, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_FOG_OF_WAR, this);
	EntityFactory::Destroy();
	EntityId::Destroy();
	PollingStation::Destroy();
	LUA::ScriptSystem::GetInstance()->Destroy();
}

void Level::LoadTutorial(const Prism::Camera& aCamera, const std::string& aTutorialPath)
{
	myTutorial = new Tutorial(aTutorialPath, myPlayer, aCamera.GetOrientation(), myNeutralDirector);
}

bool Level::Update(float aDeltaTime, Prism::Camera& aCamera)
{
	//CU::Vector3<float> lightDir(myLight->GetCurrentDir().x, myLight->GetCurrentDir().y, myLight->GetCurrentDir().z);
	//myLight->SetDir(lightDir * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f * aDeltaTime / 3.f));


	PollingStation::GetInstance()->CleanUp();
	myPlayer->CleanUp();
	myAI->CleanUp();
	myNeutralDirector->CleanUp();

	/*Prism::RenderLine3D({ 0.f, 0.f, 0.f }, { 100.f, 100.f, 100.f }, eColorDebug::BLACK, eColorDebug::GREEN);
	Prism::RenderBox({ 128.f, 129.f, 128.f }, eColorDebug::BLUE, false);*/
	//DEBUG_PRINT(myEntities[0]->GetOrientation().GetPos());

	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_F2))
	//{
	//	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::COMPLETE_LEVEL));
	//}

	myTutorial->Update(aDeltaTime);

	for (int i = 0; i < myEntities.Size(); ++i)
	{
		myEntities[i]->Update(aDeltaTime);
	}


	myPlayer->Update(aDeltaTime, aCamera);
	myAI->Update(aDeltaTime);
	myNeutralDirector->Update(aDeltaTime);

	FogOfWarMap::GetInstance()->Update(aDeltaTime);
	DoFogCulling();

	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());

	return true;
}

void Level::Render(Prism::Camera& aCamera)
{
	Prism::Engine::GetInstance()->SetClearColor({ 0.2f, 0.2f, 0.2f, 1.f });

	if (myShowFogOfWar == true)
	{
		FogOfWarMap::GetInstance()->UpdateRenderPlane();
		myFogOfWarHelper->Render(aCamera, FogOfWarMap::GetInstance()->GetRenderPlane());
	}

	myRenderer->BeginScene();
	myScene->Render(myRenderNavMeshLines, myFogOfWarHelper->GetTexture());
	myEmitterManager->RenderEmitters();

	myAI->RenderMaps(aCamera);

	if (myShowFogOfWar == true)
	{
		myRenderer->EndScene(Prism::ePostProcessing::BLOOM | Prism::ePostProcessing::FOG_OF_WAR, myFogOfWarHelper->GetTexture());
	}
	else
	{
		myRenderer->EndScene(Prism::ePostProcessing::BLOOM, myFogOfWarHelper->GetTexture());
	}
	myRenderer->FinalRender();

	
	myPlayer->RenderHealthBars(aCamera);
	myAI->RenderHealthBars(aCamera);
	myNeutralDirector->RenderHealthBars(aCamera);
	
	myPlayer->Render(aCamera);
}

void Level::OnResize(int aWidth, int aHeigth)
{
	myPlayer->OnResize(aWidth, aHeigth);
	myRenderer->OnResize(static_cast<float>(aWidth), static_cast<float>(aHeigth));
	myFogOfWarHelper->OnResize(static_cast<float>(aWidth), static_cast<float>(aHeigth));
}

void Level::ReceiveMessage(const ToggleRenderLinesMessage& aMessage)
{
	if (aMessage.myMessageType == eMessageType::TOGGLE_RENDER_LINES)
	{
		myRenderNavMeshLines = aMessage.myToggleFlag;
	}
}

void Level::ReceiveMessage(const ToggleFogOfWarMessage& aMessage)
{
	FogOfWarMap::GetInstance()->ToggleFogOfWar();
	myShowFogOfWar = !myShowFogOfWar;
}

void Level::SpawnUnit(eUnitType aUnitType)
{
	myPlayer->SpawnUnit(aUnitType);
}

bool Level::HasPlayerWon() const
{
	return myPlayer->GetVictoryPoints() >= myMaxVictoryPoint;
}

bool Level::HasAIWon() const
{
	return myAI->GetVictoryPoints() >= myMaxVictoryPoint;
}

CU::Vector3<float> Level::GetCameraMoveVector() const
{
	return myPlayer->GetCameraMoveVector();
}

void Level::DoFogCulling()
{
	const CU::GrowingArray<Entity*>& enemies = PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY);
	for (int i = 0; i < enemies.Size(); ++i)
	{
		enemies[i]->SetShouldRender(false);
		if (myShowFogOfWar == false || FogOfWarMap::GetInstance()->IsVisible(enemies[i]->GetPosition()))
		{
			enemies[i]->SetShouldRender(true);
		}
	}

	const CU::GrowingArray<Entity*>& neutrals = PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL);
	for (int i = 0; i < neutrals.Size(); ++i)
	{
		neutrals[i]->SetShouldRender(false);
		if (myShowFogOfWar == false || FogOfWarMap::GetInstance()->IsVisible(neutrals[i]->GetPosition()))
		{
			neutrals[i]->SetShouldRender(true);
		}
	}
}