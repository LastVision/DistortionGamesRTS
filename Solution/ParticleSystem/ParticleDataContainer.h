#pragma once
#include <string>
#include <unordered_map>

namespace Prism
{
	class ParticleEmitterData;
	class StreakEmitterData;


	class ParticleDataContainer
	{

	public:
		static ParticleDataContainer* GetInstance();
		static void Destroy();

		ParticleEmitterData*	GetParticleData(const char* aFilePath);

	private:
		ParticleDataContainer(){};
		~ParticleDataContainer();
		void LoadParticleData(const char* aFilePath);

		std::unordered_map<const char*, ParticleEmitterData*> myParticleData;

		static ParticleDataContainer* myInstance;
	};
};