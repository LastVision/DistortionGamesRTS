#pragma once
#include "Component.h"

namespace Prism
{
	class ParticleEmitterInstance;
}

class ParticleEmitterComponent : public Component
{
public:
	ParticleEmitterComponent(Entity& aEntity);
	~ParticleEmitterComponent();
	void Init(const std::string& aPath);
	void Update(float aDeltaTime) override;
	void Render();
	static eComponentType GetType();
	int GetEmitterCount();
	Prism::ParticleEmitterInstance* GetEmitter(int anIndex = 0);

	void ReceiveNote(const EmitterNote& aNote) override;


private:
	void ReadList(const std::string& aPath);
	void AddEmitter(const std::string& aPath);
	struct Emitter
	{
		~Emitter();
		Prism::ParticleEmitterInstance* myEmitter;
		//CU::Matrix44f myOrientation;
	};

	CU::GrowingArray<Emitter*> myEmitters;

	std::string myXMLPath;
};

inline eComponentType ParticleEmitterComponent::GetType()
{
	return eComponentType::PARTICLE_EMITTER;
}