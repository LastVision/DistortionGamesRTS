#pragma once
#include "Component.h"
#include "EnrageComponentData.h"
class EnrageComponent :
	public Component
{
public:
	EnrageComponent(Entity& anEntity, EnrageComponentData& aData);
	~EnrageComponent();

	void Update(float aDeltaTime);

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();
	bool IsActive();
	void Activate();

private:

	float myCurrentDuration;

	float myCurrentCooldown;

	bool myIsActive;

	EnrageComponentData myData;

	float myOriginalArmor;
	//float myOriginalHealth;
	float myOriginalAttackDamage;
	float myOriginalRechargeTime;
	float myOriginalAttackRange2;
	float myOriginalMovementSpeed;

};

inline eComponentType EnrageComponent::GetTypeStatic()
{
	return eComponentType::ENRAGE;
}

inline eComponentType EnrageComponent::GetType()
{
	return GetTypeStatic();
}

inline bool EnrageComponent::IsActive()
{
	return myIsActive;
}