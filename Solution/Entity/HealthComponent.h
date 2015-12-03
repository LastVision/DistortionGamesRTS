#pragma once
#include "Component.h"

struct HealthComponentData;
class HealthComponent : public Component
{
public:
	HealthComponent(Entity& aEntity, HealthComponentData& aData);
	~HealthComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	int myHealth;
};

inline eComponentType HealthComponent::GetTypeStatic()
{
	return eComponentType::HEALTH;
}

inline eComponentType HealthComponent::GetType()
{
	return GetTypeStatic();
}