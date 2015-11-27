#include "stdafx.h"

#include <Camera.h>
#include <DirectionalLight.h>
#include <Engine.h>
#include <EngineEnums.h>
#include "Level.h"
#include <Scene.h>
#include <Terrain.h>


#include <Entity.h>
#include <GraphicsComponent.h>
#include <AnimationComponent.h>
#include <InputWrapper.h>
#include <MovementComponent.h>

Level::Level(const Prism::Camera& aCamera)
{
	myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/playground.tga"
		, "Data/Resource/Texture/Terrain/T_rock.dds", { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());

	myScene = new Prism::Scene(aCamera, *myTerrain);
	myUnits.Init(20);
	for (int i = 0; i < 1; ++i)
	{
		//Entity* unit = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "Dragon", { 60.f + i * 5, 40, 200 });
		Entity* unit = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "Dragon", { 0.f, 0.f, 0.f });

		unit->AddComponent(new AnimationComponent(*unit, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_idle.fbx"
			, "Data/Resource/Shader/S_effect_no_texture_animated.fx"));
		unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::IDLE
			, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_idle.fbx", true, false);
		unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::WALKING
			, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_runcycle.fbx", true, false);
		unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::ATTACKING
			, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_attack.fbx", false, true);
		unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::DYING
			, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_death.fbx", false, true);

		unit->AddComponent(new MovementComponent(*unit, 20.f, *myTerrain));

		myScene->AddInstance(unit->GetComponent<AnimationComponent>()->GetInstance());
		myUnits.Add(unit);
	}

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
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_5))
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			myUnits[i]->SetState(eEntityState::NO_ANIMATION);
		}
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_P))
	{
		for (int i = 1; i < 26; ++i)
		{
			int zRand = rand() % 100;
			Entity* unit = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "Dragon", { 60.f + i * 5, 40, zRand*1.f });

			unit->AddComponent(new AnimationComponent(*unit, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_idle.fbx"
				, "Data/Resource/Shader/S_effect_no_texture_animated.fx"));
			unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::IDLE
				, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_idle.fbx", true, false);
			unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::WALKING
				, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_runcycle.fbx", true, false);
			/*unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::ATTACKING
				, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_attack.fbx", false, true);
			unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::DYING
				, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_death.fbx", false, true);*/

			myScene->AddInstance(unit->GetComponent<AnimationComponent>()->GetInstance());
			myUnits.Add(unit);
		}
	}

	if (CU::InputWrapper::GetInstance()->MouseDown(1))
	{
		myUnits[0]->GetComponent<MovementComponent>()->SetWayPoints(myWaypoints);
		myWaypoints.RemoveAll();
	}

	if (CU::InputWrapper::GetInstance()->MouseDown(0))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		myWaypoints.Add({ newPos.x, 0.f, newPos.z });
	}
	if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) && CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		myWaypoints.Add({ newPos.x, 0.f, newPos.z });
	}
	

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

CU::Vector3<float> Level::CalcCursorWorldPosition(Prism::Camera& aCamera)
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

	return worldPos;
}