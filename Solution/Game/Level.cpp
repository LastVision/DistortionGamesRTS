#include "stdafx.h"

#include <AnimationComponent.h>
#include <Camera.h>
#include <CollisionComponent.h>
#include <DirectionalLight.h>
#include <EffectContainer.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <GraphicsComponent.h>
#include <InputWrapper.h>
#include <Intersection.h>
#include "Level.h"
#include <ModelLoader.h>
#include "PlayerDirector.h"
#include <Scene.h>
#include <Terrain.h>


Level::Level(const Prism::Camera& aCamera, Prism::Terrain* aTerrain, GUI::Cursor* aCursor)
	: myEntities(64)
{
	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_entity.xml");
	myTerrain = aTerrain;
	/*myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/playground.tga"
		, "Data/Resource/Texture/Terrain/T_rock.dds", { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());*/

	myScene = new Prism::Scene(aCamera, *myTerrain);
	Prism::ModelLoader::GetInstance()->Pause();
	Prism::EffectContainer::GetInstance()->SetCubeMap("Data/Resource/Texture/CubeMap/T_cubemap_level01.dds");
	Prism::ModelLoader::GetInstance()->UnPause();
	myLight = new Prism::DirectionalLight();
	myLight->SetColor({ 0.5f, 0.5f, 0.9f, 1.f });
	myLight->SetDir(CU::Vector3<float>(0, 1, 0) * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f / 3.f));
	myScene->AddLight(myLight);

	myPlayer = new PlayerDirector(*myTerrain, *myScene);
}

Level::~Level()
{
	myEntities.DeleteAll();
	SAFE_DELETE(myTerrain);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myLight);
	SAFE_DELETE(myPlayer);

	EntityFactory::Destroy();
}

bool Level::Update(float aDeltaTime, Prism::Camera& aCamera)
{
	//CU::Vector3<float> lightDir(myLight->GetCurrentDir().x, myLight->GetCurrentDir().y, myLight->GetCurrentDir().z);
	//myLight->SetDir(lightDir * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f * aDeltaTime / 3.f));

	Prism::RenderLine3D({ 0.f, 0.f, 0.f }, { 100.f, 100.f, 100.f }, eColorDebug::BLACK, eColorDebug::GREEN);
	Prism::RenderBox({ 128.f, 129.f, 128.f }, eColorDebug::BLUE, false);
	DEBUG_PRINT(myEntities[0]->GetOrientation().GetPos());

	myPlayer->Update(aDeltaTime, aCamera);

	return true;
}

void Level::Render()
{
	Prism::Engine::GetInstance()->SetClearColor({ 0.2f, 0.2f, 0.2f, 1.f });

	myScene->Render();

	myPlayer->Render();
}

void Level::OnResize(int aWidth, int aHeigth)
{
	myPlayer->OnResize(aWidth, aHeigth);
}