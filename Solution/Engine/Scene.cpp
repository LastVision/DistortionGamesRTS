#include "stdafx.h"

#include <AABB.h>
#include "Camera.h"
#include "Defines.h"
#include "DirectionalLight.h"
#include "EngineEnums.h"
#include "Instance.h"
#include "PointLight.h"
#include "Scene.h"
#include "SpotLight.h"
#include <XMLReader.h>
#ifdef SCENE_USE_OCTREE
#include "Octree.h"
#endif

Prism::Scene::Scene()
#ifdef SCENE_USE_OCTREE
	: myOctree(new Octree(6))
	, myPlayerInstance(nullptr)
#endif
{
	myInstances.Init(4096);
	myDynamicInstances.Init(64);
	myDirectionalLights.Init(4);
	myPointLights.Init(4);
	mySpotLights.Init(4);

	memset(&myDirectionalLightData[0], 0, sizeof(DirectionalLightData) * NUMBER_OF_DIRECTIONAL_LIGHTS);
	memset(&myPointLightData[0], 0, sizeof(PointLightData) * NUMBER_OF_POINT_LIGHTS);
	memset(&mySpotLightData[0], 0, sizeof(SpotLightData) * NUMBER_OF_SPOT_LIGHTS);

	
}

Prism::Scene::~Scene()
{

#ifdef SCENE_USE_OCTREE
	delete myOctree;
	myOctree = nullptr;

#else
	myInstances.DeleteAll();
#endif
}

void Prism::Scene::Render()
{
	for (int i = 0; i < myDirectionalLights.Size(); ++i)
	{
		myDirectionalLights[i]->Update();

		myDirectionalLightData[i].myDirection = myDirectionalLights[i]->GetCurrentDir();
		myDirectionalLightData[i].myColor = myDirectionalLights[i]->GetColor();
	}

	for (int i = 0; i < myPointLights.Size(); ++i)
	{
		myPointLights[i]->Update();

		myPointLightData[i].myColor = myPointLights[i]->GetColor();
		myPointLightData[i].myPosition = myPointLights[i]->GetPosition();
		myPointLightData[i].myRange = myPointLights[i]->GetRange();
	}

	for (int i = 0; i < mySpotLights.Size(); ++i)
	{
		mySpotLights[i]->Update();

		mySpotLightData[i].myPosition = mySpotLights[i]->GetPosition();
		mySpotLightData[i].myDirection = mySpotLights[i]->GetDir();
		mySpotLightData[i].myColor = mySpotLights[i]->GetColor();
		mySpotLightData[i].myRange = mySpotLights[i]->GetRange();
		mySpotLightData[i].myCone = mySpotLights[i]->GetCone();
	}

	myPlayerInstance->UpdateDirectionalLights(myDirectionalLightData);
	myPlayerInstance->UpdatePointLights(myPointLightData);
	myPlayerInstance->UpdateSpotLights(mySpotLightData);
	//myPlayerInstance->Render(*myCamera);

#ifdef SCENE_USE_OCTREE
	myOctree->Update();
	myInstances.RemoveAll();
	myOctree->GetOccupantsInAABB(myCamera->GetFrustum(), myInstances);

	for (int i = 0; i < myDynamicInstances.Size(); ++i)
	{
		myDynamicInstances[i]->UpdateDirectionalLights(myDirectionalLightData);
		myDynamicInstances[i]->UpdatePointLights(myPointLightData);
		myDynamicInstances[i]->UpdateSpotLights(mySpotLightData);
		myDynamicInstances[i]->Render(*myCamera);
	}
#ifdef SHOW_OCTREE_DEBUG
	Engine::GetInstance()->PrintText(myInstances.Size(), { 600.f, -600.f });
#endif
#endif
	for (int i = 0; i < myInstances.Size(); ++i)
	{
		myInstances[i]->UpdateDirectionalLights(myDirectionalLightData);
		myInstances[i]->UpdatePointLights(myPointLightData);
		myInstances[i]->UpdateSpotLights(mySpotLightData);
		myInstances[i]->Render(*myCamera);
	}
}

void Prism::Scene::Render(CU::GrowingArray<Instance*>& someBulletInstances)
{
	Render();

	for (int i = 0; i < someBulletInstances.Size(); ++i)
	{
		someBulletInstances[i]->UpdateDirectionalLights(myDirectionalLightData);
		someBulletInstances[i]->UpdatePointLights(myPointLightData);
		someBulletInstances[i]->UpdateSpotLights(mySpotLightData);
		someBulletInstances[i]->Render(*myCamera);
	}
}

void Prism::Scene::RenderCockpit()
{
	myPlayerInstance->Render(*myCamera);
}

void Prism::Scene::AddInstance(Instance* aInstance)
{
#ifdef SCENE_USE_OCTREE
	if (aInstance->GetOctreeType() == eOctreeType::PLAYER)
	{
		DL_ASSERT_EXP(myPlayerInstance == nullptr, "Tried to add Player twice to Scene");
		myPlayerInstance = aInstance;
	}
	else if (aInstance->GetOctreeType() == eOctreeType::DYNAMIC)
	{
		myDynamicInstances.Add(aInstance);
	}
	else
	{
		myOctree->Add(aInstance);
	}
#else
	myInstances.Add(aInstance);
#endif
}

void Prism::Scene::AddLight(DirectionalLight* aLight)
{
	myDirectionalLights.Add(aLight);
}

void Prism::Scene::AddLight(PointLight* aLight)
{
	myPointLights.Add(aLight);
}

void Prism::Scene::AddLight(SpotLight* aLight)
{
	mySpotLights.Add(aLight);
}

void Prism::Scene::RemoveInstance(Instance* aInstance) 
{
#ifdef SCENE_USE_OCTREE
	if (aInstance->GetOctreeType() == eOctreeType::DYNAMIC)
	{
		myDynamicInstances.RemoveCyclic(aInstance);
	}
	else
	{
		myOctree->Remove(aInstance);
	}
#else
	myInstances.RemoveCyclic(aInstance);
#endif
}

void Prism::Scene::SetCamera(Camera* aCamera)
{
	myCamera = aCamera;
}

Prism::Camera& Prism::Scene::GetCamera()
{
	return *myCamera;
}