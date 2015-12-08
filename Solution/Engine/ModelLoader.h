#pragma once

#include <GrowingArray.h>
#include <atomic>

namespace Prism
{
	class AnimationProxy;
	class Model;
	class ModelProxy;
	class FBXFactory;
	class DGFXLoader;

	class ModelLoader
	{
	public:
		static ModelLoader* GetInstance();
		static void Destroy();

		void Run();
		void Shutdown();
		void ClearLoadJobs();

		volatile bool IsLoading() const;
		void Pause();
		void UnPause();
		void WaitUntilFinished() const;

		ModelProxy* LoadModel(const std::string& aModelPath, const std::string& aEffectPath);
		ModelProxy* LoadModelAnimated(const std::string& aModelPath, const std::string& aEffectPath);
		ModelProxy* LoadCube(float aWidth = 1.f, float aHeight = 1.f, float aDepth = 1.f
			, CU::Vector4f aColour = { 1.f, 1.f, 1.f, 1.f });

		AnimationProxy* LoadAnimation(const char* aPath);

	private:
		enum class eLoadType
		{
			MODEL,
			MODEL_ANIMATED,
			ANIMATION,
			CUBE,
		};
		struct LoadData
		{
			ModelProxy* myProxy;
			eLoadType myLoadType;
			AnimationProxy* myAnimationProxy;
			std::string myModelPath = "";
			std::string myEffectPath = "";
			CU::Vector3<float> mySize;
			CU::Vector4<float> myColor;
		};

		ModelLoader();
		~ModelLoader();

		void WaitUntilCopyIsAllowed();
		void WaitUntilAddIsAllowed();
		void AddPrefetchJobs();

		CU::GrowingArray<LoadData> myBuffers[2];
		CU::GrowingArray<LoadData> myLoadArray;
		int myActiveBuffer;
		int myInactiveBuffer;
		volatile bool myCanAddToLoadArray;
		volatile bool myCanCopyArray;
		volatile bool myIsRunning;
		volatile bool myIsLoading;
		volatile bool myClearLoadJobs;
		volatile bool myIsPaused;
		volatile bool myHasPrefetched;

		FBXFactory* myModelFactory;
		DGFXLoader* myDGFXLoader;
		CU::GrowingArray<Model*> myNonFXBModels;
		std::unordered_map<std::string, ModelProxy*> myProxies;

		static ModelLoader* myInstance;
	};
}