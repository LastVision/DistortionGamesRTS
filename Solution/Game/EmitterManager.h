#pragma once
#include "Subscriber.h"
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
	std::unordered_map<std::string, EmitterData*> myEmitters;
};

