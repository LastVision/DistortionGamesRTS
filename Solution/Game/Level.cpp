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
#include <MovementComponent.h>
#include <Scene.h>
#include <Terrain.h>



Level::Level(const Prism::Camera& aCamera)
{
	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_entity.xml");

	myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/playground.tga"
		, "Data/Resource/Texture/Terrain/T_rock.dds", { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());
	
	myScene = new Prism::Scene(aCamera, *myTerrain);
	myUnits.Init(20);
	for (int i = 0; i < 15; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			*myScene, { 20.f + i, 0.f, 20.f }, *myTerrain));
	}
	Prism::ModelLoader::GetInstance()->Pause();
	Prism::EffectContainer::GetInstance()->SetCubeMap("Data/Resource/Texture/CubeMap/T_cubemap_level01.dds");
	Prism::ModelLoader::GetInstance()->UnPause();

	myWaypoints.Init(4);
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
	Prism::RenderBox({ 128.f, 129.f, 128.f }, eColorDebug::BLUE, false);


	UpdateMouseInteraction(aCamera);

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

CU::Vector3<float> Level::CalcCursorWorldPosition(Prism::Camera& aCamera)
{
	CU::Vector2<float> inputPos(CU::InputWrapper::GetInstance()->GetMousePosition());
	CU::Vector2<float> cursorPos;
	CU::Vector2<float> window = Prism::Engine::GetInstance()->GetWindowSize();

	float tweakValue = 1.85f; // for 16:9
	float epsilon = 0.1f;
	float aspect = window.x / window.y;
	if (aspect <= 5.f / 4.f + epsilon)
	{
		tweakValue = 1.255f;
	}
	else if (aspect <= 16.f / 10.f + epsilon)
	{
		tweakValue = 1.605f;
	}

	float padding = (window.x - window.y) * 0.5f;
	float mult = window.y / window.x;
	cursorPos = inputPos;

	cursorPos.y = window.y - cursorPos.y;
	cursorPos.y /= window.y;
	cursorPos.y *= mult / tweakValue;
	cursorPos.y += (1.f - mult / tweakValue) / 2.f;

	cursorPos.x /= window.x;

	Prism::Engine::GetInstance()->PrintText(cursorPos.x, { 50.f, 50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(cursorPos.y, { 280.f, 50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(tweakValue, { 480.f, 50.f }, Prism::eTextType::DEBUG_TEXT);

	CU::Vector3<float> worldPos(myTerrain->CalcIntersection(aCamera.GetOrientation().GetPos()
		, aCamera.RayCast(cursorPos)));
	
	//Debug:
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });

	return worldPos;
}

void Level::UpdateMouseInteraction(Prism::Camera& aCamera)
{
	CU::Vector3<float> targetPos = CalcCursorWorldPosition(aCamera);
	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), targetPos);

	bool leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0);
	bool hasSelected = false;
	bool hasHovered = false;
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		bool mouseOnUnit = myUnits[i]->GetComponent<CollisionComponent>()->Collide(line);

		if (leftClicked == true)
		{
			myUnits[i]->SetSelect(false);
		}

		myUnits[i]->SetHovered(false);

		if (mouseOnUnit == true)
		{
			if (leftClicked == true && hasSelected == false)
			{
				myUnits[i]->SetSelect(true);
				hasSelected = true;
			}
			else if (hasHovered == false)
			{
				myUnits[i]->SetHovered(true);
				hasHovered = true;
			}
		}
	}

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) && CU::InputWrapper::GetInstance()->MouseDown(1))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->IsSelected())
			{
				myUnits[i]->GetComponent<MovementComponent>()->AddWayPoint(newPos, false);
			}
		}
	}
	else if (CU::InputWrapper::GetInstance()->MouseDown(1))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->IsSelected())
			{
				myUnits[i]->GetComponent<MovementComponent>()->AddWayPoint(newPos, true);
			}
		}
	}
}