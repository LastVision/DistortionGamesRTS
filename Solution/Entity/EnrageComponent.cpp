#include "stdafx.h"
#include "ActorComponent.h"
#include "ControllerComponent.h"
#include "EnrageComponent.h"
#include "HealthComponent.h"

EnrageComponent::EnrageComponent(Entity& anEntity, EnrageComponentData& aData)
	: Component(anEntity)
	, myCurrentCooldown(0.f)
	, myCurrentDuration(0.f)
	, myIsActive(false)
	, myData(aData)
{
}

EnrageComponent::~EnrageComponent()
{
}

void EnrageComponent::Update(float aDeltaTime)
{
	myCurrentCooldown -= aDeltaTime;
	if (myIsActive == true)
	{
		myCurrentDuration -= aDeltaTime;

		if (myCurrentDuration <= 0.f)
		{
			myIsActive = false;

			/*
				Revert the modifiers that were activated when enrage activated.
				*/

		}
	}
}

void EnrageComponent::Activate()
{
	if (myCurrentCooldown <= 0.f)
	{
		HealthComponent* health = myEntity.GetComponent<HealthComponent>();
		ControllerComponent* controller = myEntity.GetComponent<ControllerComponent>();
		ActorComponent* actor = myEntity.GetComponent<ActorComponent>();


		myIsActive = true;
		myCurrentDuration = myData.myDuration;
		myCurrentCooldown = myData.myCooldown;

		myOriginalArmor = health->GetArmor();
		myOriginalMovementSpeed = actor->GetSpeed();
		myOriginalAttackDamage = controller->GetAttackDamage();
		myOriginalAttackRange2 = controller->GetAttackRange2();
		myOriginalAttackSpeed = controller->GetAttackSpeed();


		health->SetHealth(health->GetCurrentHealth() - (health->GetMaxHealth()
			* ((myData.myHealthModifier + 100.f) / 100.f)));
		health->SetArmor(myOriginalArmor * ((myData.myArmorModifier + 100.f) / 100.f));
		actor->SetSpeed(myOriginalMovementSpeed * ((myData.myMovementSpeedModifier + 100.f) / 100.f));

		myOriginalAttackDamage = controller->GetAttackDamage();
		myOriginalAttackRange2 = controller->GetAttackRange2();
		myOriginalAttackSpeed = controller->GetAttackSpeed();

	}
}