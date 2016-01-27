#pragma once

#include <GrowingArray.h>
#include <unordered_map>
#include <GameEnum.h>



namespace Prism
{
	class Camera;
	class Model;

	struct ModelData
	{
		CU::GrowingArray<CU::Matrix44<float>> myMatrices;
		CU::GrowingArray<CU::Vector3<float>> myScales;
	};
	typedef std::unordered_map<Model*, ModelData> ModelMap;

	class InstancingHelper
	{
	public:
		InstancingHelper();

		void SetCamera(const Camera* aCamera);
		void AddModel(eOwnerType aOwner, Model* aModel, const CU::Matrix44<float>& aWorldMatrix
			, const CU::Vector3<float>& aScale);

		void Render(CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someLights);

	private:
		const Camera* myCamera;
		std::unordered_map<eOwnerType, ModelMap> myRenderInfo;
	};
}