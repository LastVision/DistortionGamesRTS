#pragma once

#include "Component.h"

namespace Prism
{
	class Terrain;
}

struct ActorComponentData;

class ActorComponent : public Component
{
public:
	ActorComponent(Entity& aEntity, ActorComponentData& aData, const Prism::Terrain& aTerrain);
	~ActorComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	void LookAt(const CU::Vector3<float>& aDirection);
	float mySpeed;
	const Prism::Terrain& myTerrain;
};

inline eComponentType ActorComponent::GetTypeStatic()
{
	return eComponentType::ACTOR;
}

inline eComponentType ActorComponent::GetType()
{
	return GetTypeStatic();
}
