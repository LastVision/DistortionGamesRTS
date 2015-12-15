#pragma once
#include "Subscriber.h"
#include <StaticArray.h>
#define PREALLOCATED_EMITTER_SIZE 128
class Entity;
class ParticleEmitterComponent;

namespace Prism
{
	class ParticleEmitterInstance;
	class Camera;
};

struct EmitterData
{
	EmitterData(std::string aType);
	~EmitterData();

	CU::StaticArray<Prism::ParticleEmitterInstance*, PREALLOCATED_EMITTER_SIZE> myEmitterA;
	CU::StaticArray<Prism::ParticleEmitterInstance*, PREALLOCATED_EMITTER_SIZE> myEmitterB;
	CU::StaticArray<Prism::ParticleEmitterInstance*, PREALLOCATED_EMITTER_SIZE> myEmitterC;

	std::string myType;

	int myEmitterIndex;
};


class EmitterManager : public Subscriber
{
public:
	EmitterManager();
	~EmitterManager();

	//void AddEmitter(ParticleEmitterComponent* anEmitter);


	void UpdateEmitters(float aDeltaTime, CU::Matrix44f aWorldMatrix);
	void RenderEmitters(Prism::Camera* aCamera);

	/*void ReceiveMessage(const DestroyEmitterMessage& aMessage) override;
	void ReceiveMessage(const SpawnExplosionMessage& aMessage) override;*/
	void ReceiveMessage(const EmitterMessage& aMessage) override;

private:

	void ReadListOfLists(std::string aPath);
	void ReadList(std::string aPath);

	CU::GrowingArray<std::string> myXMLPaths;

	CU::StaticArray<EmitterData*, static_cast<int>(eParticleType::_COUNT)> myEmitters;

	/*Entity* myPlayer;

	bool myIsCloseToPlayer;
	*/
	/*
	enum class eTYPE
	{
		EXPLOSION,
		SMOKE,
		SPARK,
		COUNT
	};*/
};

