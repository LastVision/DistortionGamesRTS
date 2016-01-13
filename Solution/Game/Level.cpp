#include "stdafx.h"

#include "AIDirector.h"
#include <AnimationComponent.h>
#include <Camera.h>
#include <CollisionComponent.h>
#include <DirectionalLight.h>
#include <EffectContainer.h>
#include "EmitterManager.h"
#include <Engine.h>
#include <EngineEnums.h>
#include <Entity.h>
#include <EntityId.h>
#include <EntityFactory.h>
#include <GameStateMessage.h>
#include <GraphicsComponent.h>
#include <InputWrapper.h>
#include <Intersection.h>
#include "Level.h"
#include <MoveCameraMessage.h>
#include <ModelLoader.h>
#include <PathFinderAStar.h>
#include <PathFinderFunnel.h>
#include "NeutralDirector.h"
#include "PlayerDirector.h"
#include "PollingStation.h"
#include <PostMaster.h>
#include <Scene.h>
#include <ScriptSystem.h>
#include <Terrain.h>
#include <ToggleRenderLinesMessage.h>
#include "Tutorial.h"

Level::Level(const Prism::Camera& aCamera, Prism::Terrain* aTerrain, GUI::Cursor* aCursor)
	: myEntities(64)
	, myRenderNavMeshLines(false)
	, myMaxVictoryPoint(-1)
{
	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_entity.xml");
	myEmitterManager = new EmitterManager(aCamera);
	myTerrain = aTerrain;

	myScene = new Prism::Scene(aCamera, *myTerrain);

	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_RENDER_LINES, this);

	myPlayer = new PlayerDirector(*myTerrain, *myScene, aCursor);
	myAI = new AIDirector(*myTerrain, *myScene);
	myNeutralDirector = new NeutralDirector(*myTerrain, *myScene);
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
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_RENDER_LINES, this);
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
	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());


	return true;
}

void Level::Render(Prism::Camera& aCamera)
{
	Prism::Engine::GetInstance()->SetClearColor({ 0.2f, 0.2f, 0.2f, 1.f });

	myScene->Render(myRenderNavMeshLines);

	myEmitterManager->RenderEmitters();

	myPlayer->RenderHealthBars(aCamera);
	myAI->RenderHealthBars(aCamera);
	myNeutralDirector->RenderHealthBars(aCamera);

	myPlayer->Render(aCamera);

	
	//const CU::GrowingArray<Prism::Navigation::Triangle*>& path = myTerrain->GetPathFinder()->GetAStar()->GetLatestPath();
	//for (int i = 0; i < path.Size() - 1; ++i)
	//{
	//	CU::Vector3<float> aStar0 = path[i]->GetCenter();
	//	CU::Vector3<float> aStar1 = path[i + 1]->GetCenter();
	//	aStar0 = myTerrain->GetHeight(aStar0, 2.f);
	//	aStar1 = myTerrain->GetHeight(aStar1, 2.f);
	//	Prism::RenderLine3D(aStar0, aStar1, eColorDebug::BLACK, eColorDebug::PINK);
	//}
}

void Level::OnResize(int aWidth, int aHeigth)
{
	myPlayer->OnResize(aWidth, aHeigth);
}

void Level::ReceiveMessage(const ToggleRenderLinesMessage& aMessage)
{
	if (aMessage.myMessageType == eMessageType::TOGGLE_RENDER_LINES)
	{
		myRenderNavMeshLines = aMessage.myToggleFlag;
	}
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

