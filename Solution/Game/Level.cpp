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

Level::Level(const Prism::Camera& aCamera)
{
	myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/playground.tga"
		, "Data/Resource/Texture/Terrain/T_rock.dds", { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());

	myScene = new Prism::Scene(aCamera, *myTerrain);
	myUnits.Init(20);
	/*for (int i = 1; i < 26; ++i)
	{
		Entity* unit = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "Dragon", { 60.f + i * 5, 40, 200 });

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

		myScene->AddInstance(unit->GetComponent<AnimationComponent>()->GetInstance());
		myUnits.Add(unit);
	}*/

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
			unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::ATTACKING
				, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_attack.fbx", false, true);
			unit->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::DYING
				, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_death.fbx", false, true);

			myScene->AddInstance(unit->GetComponent<AnimationComponent>()->GetInstance());
			myUnits.Add(unit);
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
	CU::Vector2<float> inputPos(CU::InputWrapper::GetInstance()->GetMousePosition());
	CU::Vector2<float> cursorPos;
	CU::Vector2<float> window = Prism::Engine::GetInstance()->GetWindowSizeInFloat();

	//static float tweakValue = 1.4f;
	//tweakValue -= 0.0001f;
	//float tweakValue = 1.85f; // for 16:9
	//float tweakValue = 1.255f; // for 5:4
	float tweakValue = 1.665f; //for 16:10 ?


	float padding = (window.x - window.y) * 0.5f;
	float mult = window.y / window.x;
	cursorPos = inputPos;

	cursorPos.y = window.y - cursorPos.y;
	cursorPos.y /= window.y;
	cursorPos.y *= mult / tweakValue;
	//cursorPos.y += tweakValue * padding / window.x;
	cursorPos.y += (1.f - mult / tweakValue) / 2.f;

	cursorPos.x /= window.x;

	//cursorPos.y -= padding * (window.y / window.x) * 0.3f;
	//cursorPos /= window.x;
	//cursorPos.y = 1.f - cursorPos.y;
	//cursorPos.y /= window.x / window.y;

	//SetWindowTextA(GetActiveWindow(), itoa(cursorPos.x));
	Prism::Engine::GetInstance()->PrintText(cursorPos.x, { 50.f, -50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(cursorPos.y, { 280.f, -50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(tweakValue, { 480.f, -50.f }, Prism::eTextType::DEBUG_TEXT);


	//padding /= window.x;

	//float m = padding; // y = kx + m




	////m *= 2.f;
	//m = 1.f - m;

	//float m2 = 1.f - m;

	//float k = (m2 - m) / window.x;

	//cursorPos.y = k * inputPos.y + m;

	//cursorPos.y -= padding * 0.5f;

	////float k = -m * 2.f / window.y;

	//
	////float m = 1.f;
	////float k = -m / window.y;

	////cursorPos.y = k * inputPos.y + m;






	////float aspect = window.x / window.y;
	//cursorPos.x = inputPos.x;
	//cursorPos.x /= window.x;
	////cursorPos.y = window.x - ((inputPos.y / aspect) + padding);
	//////cursorPos.y += (window.x - window.y) * 0.4f;
	////cursorPos.y /= window.x;// *aspect;

	////cursorPos.y = window.y - (inputPos.y + padding);
	////cursorPos.y /= aspect;
	////cursorPos.y /= window.y;

	CU::Vector3<float> worldPos(myTerrain->CalcIntersection(aCamera.GetOrientation().GetPos()
		, aCamera.RayCast(cursorPos)));
	
	//Debug:
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });
}