#include "stdafx.h"

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
	//for (int i = 1; i < 26; ++i)
	//{
		//Entity* Unit = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "BoxBro", { 20, 1.f, 200 });
		///*myUnit->AddComponent(new AnimationComponent(*myUnit, "Data/Resource/Model/BoxBro/boxBro_idle_anim.fbx"
		//, "Data/Resource/Shader/S_effect_no_texture_animated.fx"));*/
		//Unit->AddComponent(new AnimationComponent(*Unit, "Data/Resource/Model/multiObject_animation.fbx"
		//	, "Data/Resource/Shader/S_effect_no_texture_animated.fx"));

		//myScene->AddInstance(Unit->GetComponent<AnimationComponent>()->GetInstance());
		//myUnits.Add(Unit);
	//}
	

	myStaticUnit = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "BoxBroStatic", { -60, 20, 200 });
	myStaticUnit->AddComponent(new GraphicsComponent(*myStaticUnit, "Data/Resource/Model/BoxBro/boxBro_idle_anim.fbx"
		, "Data/Resource/Shader/S_effect_no_texture.fx"));
	
	myDragon = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "Dragon", { 60, 40, 200 });
	myDragon->AddComponent(new AnimationComponent(*myDragon, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_idle.fbx"
		, "Data/Resource/Shader/S_effect_no_texture_animated.fx"));
	myDragon->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::IDLE, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_idle.fbx");
	myDragon->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::WALKING, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_runcycle.fbx");
	myDragon->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::ATTACKING, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_attack.fbx");
	myDragon->GetComponent<AnimationComponent>()->AddAnimation(eEntityState::DYING, "Data/Resource/Model/Animated_Dragon/dragon_tier_02_death.fbx");

	//myScene->AddInstance(myUnit->GetComponent<AnimationComponent>()->GetInstance());
	myScene->AddInstance(myStaticUnit->GetComponent<GraphicsComponent>()->GetInstance());
	myScene->AddInstance(myDragon->GetComponent<AnimationComponent>()->GetInstance());


	myLight = new Prism::DirectionalLight();
	myLight->SetColor({ 0.5f, 0.5f, 0.9f, 1.f });
	myLight->SetDir(CU::Vector3<float>(0, 1, 0) * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f / 3.f));
	myScene->AddLight(myLight);
}

Level::~Level()
{
	SAFE_DELETE(myTerrain);
	SAFE_DELETE(myUnit);
	SAFE_DELETE(myStaticUnit);
	SAFE_DELETE(myDragon);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myLight);

	myUnits.DeleteAll();
}

bool Level::LogicUpdate(float aDeltaTime)
{
	//CU::Vector3<float> lightDir(myLight->GetCurrentDir().x, myLight->GetCurrentDir().y, myLight->GetCurrentDir().z);
	//myLight->SetDir(lightDir * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f * aDeltaTime / 3.f));

	Prism::RenderLine3D({ 0.f, 0.f, 0.f }, { 100.f, 100.f, 100.f }, eColorDebug::BLACK, eColorDebug::GREEN);
	Prism::RenderBox({ 128.f, 129.f, 128.f }, 2.f, eColorDebug::BLUE, false);

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1))
	{
		myDragon->SetState(eEntityState::IDLE);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_2))
	{
		myDragon->SetState(eEntityState::WALKING);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_3))
	{
		myDragon->SetState(eEntityState::ATTACKING);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4))
	{
		myDragon->SetState(eEntityState::DYING);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_5))
	{
		myDragon->SetState(eEntityState::NO_ANIMATION);
	}

	//myUnit->Update(aDeltaTime);
	myDragon->Update(aDeltaTime);
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
