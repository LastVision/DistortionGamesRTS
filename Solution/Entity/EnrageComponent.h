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
	float myOriginalAttackSpeed;
	float myOriginalAttackRange2;
	float myOriginalMovementSpeed;

};

inline bool EnrageComponent::IsActive()
{
	return myIsActive;
}