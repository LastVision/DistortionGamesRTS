#include "stdafx.h"
#include "ParticleDataContainer.h"

#include "ParticleEmitterData.h"
#include "StreakEmitterData.h"

#ifdef DLL_EXPORT
#include "Engine.h"
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

	ParticleEmitterData* ParticleDataContainer::GetParticleData(const char* aFilePath)
	{
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
			delete it->second;
			it->second = nullptr;
			myParticleData.erase(aFilePath);
		}
		LoadParticleData(aFilePath);

		return myParticleData[aFilePath];
#endif
	}

	void ParticleDataContainer::LoadParticleData(const char* aFilePath)
	{
		ParticleEmitterData* newData = new ParticleEmitterData();

		newData->LoadDataFile(aFilePath);
		DL_ASSERT_EXP(newData != nullptr, "Failed to load data. newData became nullptr.");

		myParticleData[aFilePath] = newData;

	}
}

 