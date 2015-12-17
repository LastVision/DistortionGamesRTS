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

	float GetArmor() const;
	float GetCurrentHealth() const;
	float GetMaxHealth() const;

	void SetArmor(float aArmor);
	void SetHealth(float aHealth);

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

inline float HealthComponent::GetArmor() const
{
	return myArmor;
}

inline float HealthComponent::GetCurrentHealth() const
{
	return myCurrentHealth;
}

inline float HealthComponent::GetMaxHealth() const
{
	return myMaxHealth;
}

inline void HealthComponent::SetArmor(float aArmor)
{
	myArmor = aArmor;
}

inline void HealthComponent::SetHealth(float aHealth)
{
	myCurrentHealth = aHealth;
}