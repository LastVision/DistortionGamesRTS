#include "stdafx.h"
#include "HealthComponent.h"
#include "HealthComponentData.h"


HealthComponent::HealthComponent(Entity& aEntity, HealthComponentData& aData)
	: Component(aEntity)
	, myMaxHealth(aData.myHealth)
	, myCurrentHealth(aData.myHealth)
{
}


HealthComponent::~HealthComponent()
{
}

bool HealthComponent::TakeDamage(int aDamage)
{
	DL_ASSERT_EXP(aDamage >= 0, "Cant take negative damage, use Heal for healing if that was your intention");

	myCurrentHealth -= aDamage;
	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;
		return false;
	}

	return true;
}

void HealthComponent::Heal(int aHealing)
{
	myCurrentHealth += aHealing;
}

bool HealthComponent::IsAlive() const
{
	return myCurrentHealth > 0;
}