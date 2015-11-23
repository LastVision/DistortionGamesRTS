#pragma once
#include <vector>
#include <unordered_map>
#include <GrowingArray.h>
#include <Vector.h>
#include <Matrix.h>

struct ModelData;

class FBXLoader;
class FbxModelData;

namespace Prism
{
class Model;
class Effect;
	class FBXFactory
	{
	public:
		FBXFactory();
		~FBXFactory();

		Model* LoadModel(const char* aFilePath, Effect* aEffect);
		void LoadModelForRadiusCalc(const char* aFilePath, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut);
	private:
		void FillData(ModelData* someData, Model* outData, Effect* aEffect);
		Model* CreateModel(FbxModelData* someModelData, Effect* aEffect);

		void CreateModelForRadiusCalc(FbxModelData* someModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
			, const CU::Matrix44<float>& aParentOrientation = CU::Matrix44<float>());
		void FillDataForRadiusCalc(ModelData* aModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
			, const CU::Matrix44<float>& aOrientation);

		FBXLoader *myLoader;

		struct FBXData
		{
			FbxModelData* myData;
			std::string myPath;
		};
		std::vector<FBXData*> myFBXData;
		std::unordered_map<std::string, Model*> myModels;
	};
}