#include "stdafx.h"

#include "AnimationProxy.h"
#include "Engine.h"
#include "EffectContainer.h"
#include "Model.h"
#include "ModelLoader.h"
#include "ModelProxy.h"
#include "FBXFactory.h"
#include <TimerManager.h>
#include <XMLReader.h>

namespace Prism
{
	ModelLoader* ModelLoader::myInstance = nullptr;

	ModelLoader* ModelLoader::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new ModelLoader();
		}

		return myInstance;
	}

	void ModelLoader::Destroy()
	{
		delete myInstance;
	}

	ModelLoader::ModelLoader()
		: myNonFXBModels(4)
		, myIsRunning(true)
		, myCanAddToLoadArray(true)
		, myCanCopyArray(true)
		, myModelFactory(new FBXFactory())
		, myIsLoading(false)
		, myClearLoadJobs(true)
		, myIsPaused(false)
		, myHasPrefetched(false)
	{
		myBuffers[0].Init(4096);
		myBuffers[1].Init(4096);
		myLoadArray.Init(8192);
		myActiveBuffer = 0;
		myInactiveBuffer = 1;
	}

	ModelLoader::~ModelLoader()
	{
		delete myModelFactory;
		myModelFactory = nullptr;
		myNonFXBModels.DeleteAll();

		for (auto it = myProxies.begin(); it != myProxies.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		myProxies.clear();
	}

	void ModelLoader::Run()
	{
#ifndef THREADED_LOADING
		return;
#else
		while (myIsRunning == true)
		{
			if (myHasPrefetched == false)
			{
				AddPrefetchJobs();
			}

			if (myIsPaused == true)
			{
				myCanAddToLoadArray = true;
				myIsLoading = false;
				Engine::GetInstance()->myIsLoading = false;
				std::this_thread::yield();
				continue;
			}


			WaitUntilCopyIsAllowed();
			myCanAddToLoadArray = false;

			if (myBuffers[myInactiveBuffer].Size() == 0 && myLoadArray.Size() == 0)
			{
				myCanAddToLoadArray = true;
				myIsLoading = false;
				Engine::GetInstance()->myIsLoading = false;
				std::this_thread::yield();
				continue;
			}

			if (myIsRunning == false || myClearLoadJobs == true)
			{
				myIsLoading = false;
				Engine::GetInstance()->myIsLoading = false;
				myClearLoadJobs = false;
				myCanAddToLoadArray = true;
				continue;
			}

			int oldInactive = myInactiveBuffer;
			myInactiveBuffer = myActiveBuffer;
			myActiveBuffer = oldInactive;
			myBuffers[myInactiveBuffer].RemoveAll();

			myCanAddToLoadArray = true;
			myIsLoading = true;
			Engine::GetInstance()->myIsLoading = true;

			for (int i = 0; i < myBuffers[myActiveBuffer].Size(); ++i)
			{
				myLoadArray.Add(myBuffers[myActiveBuffer][i]);
			}

			for (int i = myLoadArray.Size() - 1; i >= 0; --i)
			{
				//check in here aswell to allow early outs so we dont have to wait for 2-3 seconds to quit if
				//we got a big load-array
				if (myIsPaused == true)
				{
					break;
				}

				if (myIsRunning == false || myClearLoadJobs == true)
				{
					myIsLoading = false;
					myClearLoadJobs = false;
					myLoadArray.RemoveAll();
					break;
				}

				eLoadType loadType = myLoadArray[i].myLoadType;

				switch (loadType)
				{
				case Prism::ModelLoader::eLoadType::MODEL:
				{
					Model* model = myModelFactory->LoadModel(myLoadArray[i].myModelPath.c_str(),
						EffectContainer::GetInstance()->GetEffect(myLoadArray[i].myEffectPath));
					myLoadArray[i].myProxy->SetModel(model);

					break;
				}
				case Prism::ModelLoader::eLoadType::MODEL_ANIMATED:
				{

					ModelAnimated* model = myModelFactory->LoadModelAnimated(myLoadArray[i].myModelPath.c_str(),
						EffectContainer::GetInstance()->GetEffect(myLoadArray[i].myEffectPath));
					myLoadArray[i].myProxy->SetModelAnimated(model);

					break;
				}
				case Prism::ModelLoader::eLoadType::ANIMATION:
				{
					myLoadArray[i].myAnimationProxy->myAnimation 
						= myModelFactory->LoadAnimation(myLoadArray[i].myModelPath.c_str());
					break;
				}
				case Prism::ModelLoader::eLoadType::CUBE:
				{
					Model* model = new Prism::Model();
					model->InitCube(myLoadArray[i].mySize.x, myLoadArray[i].mySize.y,
						myLoadArray[i].mySize.z, myLoadArray[i].myColor);

					myLoadArray[i].myProxy->SetModel(model);
					myNonFXBModels.Add(model);
					break;
				}
				default:
					DL_ASSERT("ModelLoader tried to load something that dint have a specified LoadType!!!");
					break;
				}

				myLoadArray.RemoveCyclicAtIndex(i);
			}
		}
#endif
	}

	void ModelLoader::Shutdown()
	{
		myIsRunning = false;
	}

	void ModelLoader::ClearLoadJobs()
	{
		myClearLoadJobs = true;
	}

	volatile bool ModelLoader::IsLoading() const
	{
		return myIsLoading;
	}

	void ModelLoader::Pause()
	{
		myIsPaused = true;

		while (myIsLoading == true)
		{
		}
	}

	void ModelLoader::UnPause()
	{
		myIsPaused = false;
		if (myBuffers[myInactiveBuffer].Size() == 0)
		{
			return;
		}

		while (myIsLoading == false)
		{
		}
	}

	void ModelLoader::WaitUntilFinished() const
	{
		while (myIsLoading == true)
		{
		}
	}

	void ModelLoader::AddPrefetchJobs()
	{
		myHasPrefetched = true;
	}

	ModelProxy* ModelLoader::LoadModel(const std::string& aModelPath, const std::string& aEffectPath)
	{
#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();

		if (myProxies.find(aModelPath) != myProxies.end())
		{
			return myProxies[aModelPath];
		}

		myCanCopyArray = false;

		ModelProxy* proxy = new ModelProxy();
		proxy->SetModel(nullptr);

		LoadData newData;
		newData.myProxy = proxy;
		newData.myLoadType = eLoadType::MODEL;
		newData.myModelPath = aModelPath;
		newData.myEffectPath = aEffectPath;

		myBuffers[myInactiveBuffer].Add(newData);

		myProxies[aModelPath] = proxy;

		myCanCopyArray = true;

		return proxy;
#else
		if (myProxies.find(aModelPath) != myProxies.end())
		{
			return myProxies[aModelPath];
		}

		ModelProxy* proxy = new ModelProxy();

		Model* model = myModelFactory->LoadModel(aModelPath.c_str(),
			EffectContainer::GetInstance()->GetEffect(aEffectPath));

		proxy->SetModel(model);
		myProxies[aModelPath] = proxy;
		return proxy;
#endif
	}

	ModelProxy* ModelLoader::LoadModelAnimated(const std::string& aModelPath, const std::string& aEffectPath)
	{
#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();

		if (myProxies.find(aModelPath) != myProxies.end())
		{
			return myProxies[aModelPath];
		}

		myCanCopyArray = false;

		ModelProxy* proxy = new ModelProxy();
		proxy->SetModel(nullptr);

		LoadData newData;
		newData.myProxy = proxy;
		newData.myLoadType = eLoadType::MODEL_ANIMATED;
		newData.myModelPath = aModelPath;
		newData.myEffectPath = aEffectPath;

		myBuffers[myInactiveBuffer].Add(newData);

		myProxies[aModelPath] = proxy;

		myCanCopyArray = true;

		return proxy;
#else
		if (myProxies.find(aModelPath) != myProxies.end())
		{
			return myProxies[aModelPath];
		}

		ModelProxy* proxy = new ModelProxy();

		ModelAnimated* model = myModelFactory->LoadModelAnimated(aModelPath.c_str(),
			EffectContainer::GetInstance()->GetEffect(aEffectPath));

		proxy->SetModelAnimated(model);
		myProxies[aModelPath] = proxy;
		return proxy;
#endif
	}

	ModelProxy* ModelLoader::LoadCube(float aWidth, float aHeight, float aDepth
		, CU::Vector4f aColour)
	{
#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();


		myCanCopyArray = false;
		ModelProxy* proxy = new ModelProxy();
		proxy->SetModel(nullptr);

		LoadData newData;
		newData.myProxy = proxy;
		newData.myLoadType = eLoadType::CUBE;
		newData.mySize = { aWidth, aHeight, aDepth };
		newData.myColor = aColour;

		myBuffers[myInactiveBuffer].Add(newData);

		myCanCopyArray = true;

		return proxy;
#else
		ModelProxy* proxy = new ModelProxy();
		Model* model = new Prism::Model();
		model->InitCube(aWidth, aHeight, aDepth, aColour);

		proxy->SetModel(model);

		return proxy;
#endif	
	}

	AnimationProxy* ModelLoader::LoadAnimation(const char* aPath)
	{
#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();
		myCanCopyArray = false;
		AnimationProxy* anim = new AnimationProxy();

		LoadData animData;
		animData.myLoadType = eLoadType::ANIMATION;
		animData.myModelPath = aPath;
		animData.myAnimationProxy = anim;

		myBuffers[myInactiveBuffer].Add(animData);
		myCanCopyArray = true;
		return anim;
#else
		AnimationProxy* anim = new AnimationProxy();
		anim->myAnimation = myModelFactory->LoadAnimation(aPath);
		return anim;
#endif
	}

	void ModelLoader::WaitUntilCopyIsAllowed()
	{
		while (myCanCopyArray == false)
			; //Should be an empty whileloop!
	}

	void ModelLoader::WaitUntilAddIsAllowed()
	{
		while (myCanAddToLoadArray == false)
			; //Should be an empty whileloop!
	}
}