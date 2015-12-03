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

	bool TakeDamage(int aDamage);
	void Heal(int aHealing);
	bool IsAlive() const;

private:
	int myMaxHealth;
	int myCurrentHealth;
};

inline eComponentType HealthComponent::GetTypeStatic()
{
	return eComponentType::HEALTH;
}

inline eComponentType HealthComponent::GetType()
{
	return GetTypeStatic();
}