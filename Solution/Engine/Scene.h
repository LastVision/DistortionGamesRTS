#pragma once
#include "Defines.h"
#include <GrowingArray.h>
#include "LightStructs.h"
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Instance;
	class PointLight;
	class SpotLight;
#ifdef SCENE_USE_OCTREE
	class Octree;
#endif

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Render();
		void Render(CU::GrowingArray<Instance*>& someBulletInstances);
		void RenderCockpit();

		void AddInstance(Instance* aInstance);
		void AddLight(DirectionalLight* aLight);
		void AddLight(PointLight* aLight);
		void AddLight(SpotLight* aLight);

		void RemoveInstance(Instance* aInstance);

		void SetCamera(Camera* aCamera);
		Camera& GetCamera();

	private:
#ifdef SCENE_USE_OCTREE
		Octree* myOctree;
		Instance* myPlayerInstance;
#endif
		CU::GrowingArray<Instance*> myInstances;
		CU::GrowingArray<Instance*> myDynamicInstances;
		CU::GrowingArray<DirectionalLight*> myDirectionalLights;
		CU::GrowingArray<PointLight*> myPointLights;
		CU::GrowingArray<SpotLight*> mySpotLights;

		Camera* myCamera;

		CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS> myDirectionalLightData;
		CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS> myPointLightData;
		CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS> mySpotLightData;
	};
}