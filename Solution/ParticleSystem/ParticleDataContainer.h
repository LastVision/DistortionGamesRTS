#pragma once
#include <string>
#include <unordered_map>

namespace Prism
{
	class Camera;
	class ParticleEmitterData;
	
	class ParticleDataContainer
	{
	public:
		static ParticleDataContainer* GetInstance();
		static void Destroy();

		ParticleEmitterData*	GetParticleData(const char* aFilePath);
		void SetGPUData(Camera* aCamera);

	private:
		ParticleDataContainer(){};
		~ParticleDataContainer();
		void LoadParticleData(const char* aFilePath);

		std::unordered_map<const char*, ParticleEmitterData*> myParticleData;

		static ParticleDataContainer* myInstance;
	};
};