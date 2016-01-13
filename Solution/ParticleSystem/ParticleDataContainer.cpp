#include "stdafx.h"
#include "ParticleDataContainer.h"

#include "ParticleEmitterData.h"
#include <D3DX11.h>
#include <d3dx11effect.h>

#include "Camera.h"
#include "Effect.h"
#include "Engine.h"

#ifdef DLL_EXPORT
#include "FileWatcher.h"
#endif

namespace Prism
{
	ParticleDataContainer* ParticleDataContainer::myInstance = nullptr;

	ParticleDataContainer* ParticleDataContainer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new ParticleDataContainer();
		}

		return myInstance;
	}

	void ParticleDataContainer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	ParticleDataContainer::~ParticleDataContainer()
	{
		for (auto it = myParticleData.begin(); it != myParticleData.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}
	}

	ParticleEmitterData* ParticleDataContainer::GetParticleData(const std::string& aFilePath)
	{
		DL_DEBUG("Getting Particle Data");
#ifndef DLL_EXPORT
		auto it = myParticleData.find(aFilePath);

		if (it == myParticleData.end())
		{
			LoadParticleData(aFilePath);
		}

		return myParticleData[aFilePath];
#else
		auto it = myParticleData.find(aFilePath);

		if (it != myParticleData.end()) 
		{
			DL_DEBUG("Object found, removing.");
			delete it->second;
			it->second = nullptr;
			myParticleData.erase(aFilePath);
		}
		LoadParticleData(aFilePath);

		return myParticleData[aFilePath];
#endif
	}


	void ParticleDataContainer::SetGPUData(const Camera& aCamera)
	{
		DL_DEBUG("Attempting to SetGPUData");
		for (auto it = myParticleData.begin(); it != myParticleData.end(); ++it)
		{
			DL_DEBUG("Inside SetGPUData map");
			ParticleEmitterData* tempData = it->second;
			tempData->myEffect->SetViewMatrix(CU::InverseSimple(aCamera.GetOrientation()));
			tempData->myEffect->SetProjectionMatrix(aCamera.GetProjection());

			Engine::GetInstance()->GetContex()->IASetInputLayout(tempData->myInputLayout);
			Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			tempData->myEffect->GetTechnique()->GetDesc(tempData->myTechniqueDesc);
		}
		DL_DEBUG("Finished SetGPUData");
	}

	void ParticleDataContainer::LoadParticleData(const std::string& aFilePath)
	{
		DL_DEBUG("Loading particle data");
		ParticleEmitterData* newData = new ParticleEmitterData();

		newData->LoadDataFile(aFilePath);
		DL_ASSERT_EXP(newData != nullptr, "Failed to load data. newData became nullptr.");

		myParticleData[aFilePath] = newData;

	}
}

 