#pragma once

#include "Component.h"

struct ActorComponentData;

class ActorComponent : public Component
{
public:
	ActorComponent(Entity& aEntity, ActorComponentData& aData);
	~ActorComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
};

inline eComponentType ActorComponent::GetTypeStatic()
{
	return eComponentType::ACTOR;
}

inline eComponentType ActorComponent::GetType()
{
	return GetTypeStatic();
}
