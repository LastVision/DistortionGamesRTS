#pragma once
#include <string>
#include <unordered_map>

namespace Prism
{
	class ParticleEmitterData;
	class StreakEmitterData;


	class EmitterDataContainer
	{

	public:
		static EmitterDataContainer* GetInstance();
		static void Destroy();

		ParticleEmitterData*	GetParticleData(const std::string& aFilePath);
		StreakEmitterData*		GetStreakData(const std::string& aFilePath);

	private:
		EmitterDataContainer(){};
		~EmitterDataContainer();
		void LoadParticleData(const std::string& aFilePath);
		void LoadStreakData(const std::string& aFilePath);

		std::unordered_map<std::string, ParticleEmitterData*> myParticleData;
		std::unordered_map<std::string, StreakEmitterData*> myStreakData;

		static EmitterDataContainer* myInstance;
	};
};