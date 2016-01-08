#pragma once
#include "Subscriber.h"
#include <StaticArray.h>
#include <GrowingArray.h>
#include <map>
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
	EmitterData(const std::string& aType);
	~EmitterData();

	//CU::StaticArray<Prism::ParticleEmitterInstance*, PREALLOCATED_EMITTER_SIZE> myEmitterA;
	//CU::StaticArray<Prism::ParticleEmitterInstance*, PREALLOCATED_EMITTER_SIZE> myEmitterB;
	//CU::StaticArray<Prism::ParticleEmitterInstance*, PREALLOCATED_EMITTER_SIZE> myEmitterC;

	CU::GrowingArray<Prism::ParticleEmitterInstance*> myEmitters;

	std::string myType;
	std::string myIdentifier;
	int myEmitterIndex;
};


class EmitterManager : public Subscriber
{
public:
	EmitterManager();
	~EmitterManager();

	void UpdateEmitters(float aDeltaTime, CU::Matrix44f aWorldMatrix);
	void RenderEmitters(Prism::Camera* aCamera);
	void ReceiveMessage(const EmitterMessage& aMessage) override;

private:
	bool myEmittersLoaded;


	void ReadListOfLists(const std::string& aPath);
	void ReadList(const std::string& aPath, const std::string& anID);
	//CU::GrowingArray<std::string> myXMLPaths;
	//CU::GrowingArray<int> myEmitterCount;
	//CU::StaticArray<EmitterData*, static_cast<int>(eParticleType::_COUNT)> myEmitters;
	std::unordered_map<std::string, EmitterData*> myEmitters;
	//std::unordered_map<std::string, std::string> myXMLPaths;

	//std::unordered_map<std::string, int> myEmitterCount;


};

