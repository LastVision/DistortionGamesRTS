#pragma once
#include "Component.h"

class HealthComponent : public Component
{
public:
	HealthComponent(Entity& aEntity);

	void Init(int aMaxHealth, bool anIsInvulnerable = false);
	void AddHealth(int aHealthToAdd);
	void RemoveHealth(int aHealthToRemove);

	void Update(float aDeltaTime) override;

	void ReceiveNote(const PowerUpNote& aNote) override;

	bool IsAlive() const;

	int GetHealth() const;
	
	void SetInvulnerability();
	const bool& GetInvulnerability() const;
	const float& GetInvulnerablityDuration() const;


	static eComponentType GetType();

	void Reset() override;
private:

	int myMaxHealth;
	int myCurrentHealth;

	bool myIsInvulnerable;
	float myInvulnerablityTimeCurrent;
};

inline bool HealthComponent::IsAlive() const
{
	return (myCurrentHealth != 0);
}

inline int HealthComponent::GetHealth() const
{
	return myCurrentHealth;
}

inline void HealthComponent::SetInvulnerability()
{
	if (myIsInvulnerable == true)
	{
		myInvulnerablityTimeCurrent = 0.f;
		myIsInvulnerable = false;
	}
	else
	{
		myInvulnerablityTimeCurrent = 1000000000000.f; // räcker det med nollor LINUS
		myIsInvulnerable = true;
	}
}

inline const bool& HealthComponent::GetInvulnerability() const
{
	return myIsInvulnerable;
}

inline eComponentType HealthComponent::GetType()
{
	return eComponentType::HEALTH;
}

inline const float& HealthComponent::GetInvulnerablityDuration() const
{
	return myInvulnerablityTimeCurrent;
}

