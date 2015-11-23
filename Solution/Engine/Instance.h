#pragma once

#include "Defines.h"
#include "LightStructs.h"
#include <Matrix.h>
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class Light;
	class ModelProxy;
	enum class eOctreeType;

	class Instance
	{
	public:
		Instance(ModelProxy& aModel, const CU::Matrix44<float>& anOrientation, eOctreeType anOctreeType
			, const float& aObjectCullingRadius);
		~Instance();

		void Render(Camera& aCamera);
		void Render(const CU::Matrix44<float>& aParentMatrix, Camera& aCamera);

		CU::Vector3<float> GetPosition() const;

		void SetEffect(const std::string& aEffectFile);

		void SetScale(const CU::Vector3<float>& aScaleVector);

		void UpdateDirectionalLights(
			const CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData);
		void UpdatePointLights(const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData);
		void UpdateSpotLights(const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData);
		
		float GetObjectCullingRadius() const;
		eOctreeType GetOctreeType() const;

		ModelProxy& GetModel();
		const CU::Matrix44f& GetOrientation() const;
	private:
		void operator=(Instance&) = delete;

		ModelProxy& myProxy;
		const eOctreeType myOctreeType;
		const CU::Matrix44<float>& myOrientation;
		CU::Vector3<float> myScale;

		const float& myObjectCullingRadius;
	};

	inline float Instance::GetObjectCullingRadius() const
	{
		return myObjectCullingRadius;
	}

	inline eOctreeType Instance::GetOctreeType() const
	{
		return myOctreeType;
	}

	inline const CU::Matrix44f& Instance::GetOrientation() const
	{
		return myOrientation;
	}
}