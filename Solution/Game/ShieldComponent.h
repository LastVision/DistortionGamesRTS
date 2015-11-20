#pragma once
#include "Component.h"
#include "Enums.h"
class ShieldComponent : public Component
{
public:
	ShieldComponent(Entity& aEntity);


	void Init(float aTimeBeforeRecharge, float aChargeRate);
	void ReceiveNote(const PowerUpNote& aNote) override;

	void DamageShield(int someDamage);
	float GetCurrentShieldStrength();
	void Update(float aDelta) override;

	static eComponentType GetType();

	void Reset() override;

private:

	bool myShieldOvercharged;

	float myReachargeTime;
	float myChargeRate;
	float myCooldown;


	float myShieldStrength;
	float myMaxShieldStrength;
	int myOvercharge;

};

inline float ShieldComponent::GetCurrentShieldStrength()
{
	return myShieldStrength;
}

inline eComponentType ShieldComponent::GetType()
{
	return eComponentType::SHEILD;
}
