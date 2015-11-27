#include "stdafx.h"

#include <Camera.h>
#include <DirectionalLight.h>
#include <Engine.h>
#include <EngineEnums.h>
#include "Level.h"
#include <Scene.h>
#include <Terrain.h>
#include <EntityFactory.h>


#include <Entity.h>
#include <GraphicsComponent.h>
#include <AnimationComponent.h>
#include <InputWrapper.h>

Level::Level(const Prism::Camera& aCamera)
{
	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_entity.xml");

	myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/playground.tga"
		, "Data/Resource/Texture/Terrain/T_rock.dds", { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());

	myScene = new Prism::Scene(aCamera, *myTerrain);
	myUnits.Init(20);
	for (int i = 1; i < 26; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			*myScene, { 20.f + i, 20.f, 200.f }));
	}

	myLight = new Prism::DirectionalLight();
	myLight->SetColor({ 0.5f, 0.5f, 0.9f, 1.f });
	myLight->SetDir(CU::Vector3<float>(0, 1, 0) * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f / 3.f));
	myScene->AddLight(myLight);
}

Level::~Level()
{
	SAFE_DELETE(myTerrain);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myLight);

	myUnits.DeleteAll();
	EntityFactory::Destroy();
}

bool Level::LogicUpdate(float aDeltaTime, Prism::Camera& aCamera)
{
	//CU::Vector3<float> lightDir(myLight->GetCurrentDir().x, myLight->GetCurrentDir().y, myLight->GetCurrentDir().z);
	//myLight->SetDir(lightDir * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f * aDeltaTime / 3.f));

	Prism::RenderLine3D({ 0.f, 0.f, 0.f }, { 100.f, 100.f, 100.f }, eColorDebug::BLACK, eColorDebug::GREEN);
	Prism::RenderBox({ 128.f, 129.f, 128.f }, 2.f, eColorDebug::BLUE, false);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1))
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			myUnits[i]->SetState(eEntityState::IDLE);
		}
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_2))
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			myUnits[i]->SetState(eEntityState::WALKING);
		}
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_3))
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			myUnits[i]->SetState(eEntityState::ATTACKING);
		}
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4))
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			myUnits[i]->SetState(eEntityState::DYING);
		}
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_P))
	{
		for (int i = 1; i < 26; ++i)
		{
			int zRand = rand() % 100;
			myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON_STATIC, Prism::eOctreeType::DYNAMIC, 
				*myScene, { 20.f, 20.f, zRand * 1.f }));
		}
	}
	//myUnit->Update(aDeltaTime);

	CalcCursorWorldPosition(aCamera);

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		myUnits[i]->Update(aDeltaTime);
	}
	return true;
}

void Level::Render()
{
	Prism::Engine::GetInstance()->SetClearColor({ 0.2f, 0.2f, 0.2f, 1.f });

	myScene->Render();
}

void Level::OnResize(int aWidth, int aHeigth)
{
}

void Level::CalcCursorWorldPosition(Prism::Camera& aCamera)
{
	CU::Vector2<float> cursorPos;
	CU::Vector2<float> window = Prism::Engine::GetInstance()->GetWindowSizeInFloat();
	cursorPos.x = CU::InputWrapper::GetInstance()->GetMousePosition().x;
	cursorPos.x /= window.x;
	cursorPos.y = window.x - CU::InputWrapper::GetInstance()->GetMousePosition().y;
	cursorPos.y += (window.x - window.y) * 0.4f;
	cursorPos.y /= window.x * (window.x / window.y);

	CU::Vector3<float> worldPos(myTerrain->CalcIntersection(aCamera.GetOrientation().GetPos()
		, aCamera.RayCast(cursorPos)));

	//Debug:
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });
}