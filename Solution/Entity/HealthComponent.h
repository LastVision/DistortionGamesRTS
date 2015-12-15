#pragma once

namespace GUI
{
	class BarWidget;
}

struct HealthComponentData;
class HealthComponent : public Component
{
public:
	HealthComponent(Entity& aEntity, HealthComponentData& aData);
	~HealthComponent();

	void RenderHealthBar(const Prism::Camera& aCamera);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool TakeDamage(float aDamage);
	void Heal(float aHealing);

	void Reset() override;

private:
	float myMaxHealth;
	float myCurrentHealth;
	float myArmor;

	GUI::BarWidget* myHealthBar;
};

inline eComponentType HealthComponent::GetTypeStatic()
{
	return eComponentType::HEALTH;
}

inline eComponentType HealthComponent::GetType()
{
	return GetTypeStatic();
}