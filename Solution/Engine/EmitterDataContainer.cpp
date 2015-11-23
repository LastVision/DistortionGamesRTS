#include "stdafx.h"
#include "EmitterDataContainer.h"

#include "ParticleEmitterData.h"
#include "StreakEmitterData.h"

#ifdef DLL_EXPORT
#include "Engine.h"
#include "FileWatcher.h"
#endif

namespace Prism
{
	EmitterDataContainer::~EmitterDataContainer()
	{
		for (auto it = myParticleData.begin(); it != myParticleData.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}

		for (auto it = myStreakData.begin(); it != myStreakData.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}

	}

	ParticleEmitterData* EmitterDataContainer::GetParticleData(const std::string& aFilePath)
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
		
	StreakEmitterData* EmitterDataContainer::GetStreakData(const std::string& aFilePath)
	{
		auto it = myStreakData.find(aFilePath);

		if (it == myStreakData.end())
		{
			LoadStreakData(aFilePath);
		}

		return myStreakData[aFilePath];
	}

	void EmitterDataContainer::LoadParticleData(const std::string& aFilePath)
	{
		ParticleEmitterData* newData = new ParticleEmitterData();

		newData->LoadDataFile(aFilePath.c_str());
		DL_ASSERT_EXP(newData != nullptr, "Failed to load data. newData became nullptr.");

		myParticleData[aFilePath] = newData;

	}
	
	void EmitterDataContainer::LoadStreakData(const std::string& aFilePath)
	{
		StreakEmitterData* newData = new StreakEmitterData();

		newData->LoadDataFile(aFilePath.c_str());
		DL_ASSERT_EXP(newData != nullptr, "Failed to load data. newData became nullptr.");

		myStreakData[aFilePath] = newData;
	}
}

 