#include "stdafx.h"

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
		, myIsPaused(true)
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
				std::this_thread::yield();
				continue;
			}


			WaitUntilCopyIsAllowed();
			myCanAddToLoadArray = false;

			if (myBuffers[myInactiveBuffer].Size() == 0)
			{
				myCanAddToLoadArray = true;
				myIsLoading = false;
				std::this_thread::yield();
				continue;
			}

			if (myIsRunning == false || myClearLoadJobs == true)
			{
				myIsLoading = false;
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

				Model* model = nullptr;
				switch (loadType)
				{
				case Prism::ModelLoader::eLoadType::MODEL:
				{
					
					model = myModelFactory->LoadModel(myLoadArray[i].myModelPath.c_str(),
						EffectContainer::GetInstance()->GetEffect(myLoadArray[i].myEffectPath));
					
					break;
				}
				case Prism::ModelLoader::eLoadType::CUBE:
				{
					model = new Prism::Model();
					model->InitCube(myLoadArray[i].mySize.x, myLoadArray[i].mySize.y,
						myLoadArray[i].mySize.z, myLoadArray[i].myColor);

					myNonFXBModels.Add(model);
					break;
				}
				default:
					DL_ASSERT("ModelLoader tried to load something that dint have a specified LoadType!!!");
					break;
				}

				if (model == nullptr)
				{
					DL_MESSAGE_BOX("Failed to Load model", "ModelLoader->Error", MB_ICONWARNING);
				}

				myLoadArray[i].myProxy->SetModel(model);
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
		//XMLReader entityReader;
		//entityReader.OpenDocument("Data/Script/LI_list_entity.xml");
		//tinyxml2::XMLElement* entityRoot = entityReader.FindFirstChild("root");
		//tinyxml2::XMLElement* entityPathElement = entityReader.ForceFindFirstChild(entityRoot, "path");
		//for (; entityPathElement != nullptr; entityPathElement = entityReader.FindNextElement(entityPathElement, "path"))
		//{
		//	std::string entityPath;
		//	entityReader.ForceReadAttribute(entityPathElement, "src", entityPath);
		//
		//	XMLReader fbxReader;
		//	fbxReader.OpenDocument(entityPath);
		//
		//	entityRoot = fbxReader.FindFirstChild("root");
		//	tinyxml2::XMLElement* entityElement = nullptr;
		//	if (entityRoot != nullptr)
		//	{
		//		entityElement = fbxReader.ForceFindFirstChild(entityRoot, "Entity");
		//	}
		//	else
		//	{
		//		entityElement = fbxReader.ForceFindFirstChild("Entity");
		//	}
		//
		//	tinyxml2::XMLElement* gfxElement = fbxReader.FindFirstChild(entityElement, "GraphicsComponent");
		//	if (gfxElement != nullptr)
		//	{
		//		tinyxml2::XMLElement* modelElement = fbxReader.ForceFindFirstChild(gfxElement, "Model");
		//		std::string model;
		//		std::string effect;
		//
		//		fbxReader.ForceReadAttribute(modelElement, "modelFile", model);
		//		fbxReader.ForceReadAttribute(modelElement, "effectFile", effect);
		//
		//		LoadModel(model, effect);
		//	}
		//
		//	fbxReader.CloseDocument();
		//}
		//
		//entityReader.CloseDocument();

		myHasPrefetched = true;
	}

	ModelProxy* ModelLoader::LoadModel(const std::string& aModelPath, const std::string& aEffectPath)
	{
#ifdef THREADED_LOADING
		WaitUntilAddIsAllowed();

		myCanCopyArray = false;

		ModelProxy* proxy = new ModelProxy();
		proxy->SetModel(nullptr);

		LoadData newData;
		newData.myProxy = proxy;
		newData.myLoadType = eLoadType::MODEL;
		newData.myModelPath = aModelPath;
		newData.myEffectPath = aEffectPath;

		myBuffers[myInactiveBuffer].Add(newData);

		myCanCopyArray = true;

		return proxy;
#else
		ModelProxy* proxy = new ModelProxy();

		Model* model = myModelFactory->LoadModel(aModelPath.c_str(),
			EffectContainer::GetInstance()->GetEffect(aEffectPath));

		proxy->SetModel(model);

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