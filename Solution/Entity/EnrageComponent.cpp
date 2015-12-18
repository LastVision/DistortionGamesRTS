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

			HealthComponent* health = myEntity.GetComponent<HealthComponent>();
			ControllerComponent* controller = myEntity.GetComponent<ControllerComponent>();
			ActorComponent* actor = myEntity.GetComponent<ActorComponent>();

			health->SetArmor(myOriginalArmor);
			actor->SetSpeed(myOriginalMovementSpeed);
			controller->SetAttackDamage(myOriginalAttackDamage);
			controller->SetAttackRange2(myOriginalAttackRange2);
			controller->SetRechargeTime(myOriginalRechargeTime);

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
		myOriginalRechargeTime = controller->GetAttackSpeed();


		health->TakeDamage((health->GetMaxHealth() * (myData.myHealthModifier / 100.f)) + myOriginalArmor, &myEntity);
		health->SetArmor(myOriginalArmor * ((myData.myArmorModifier + 100.f) / 100.f));
		actor->SetSpeed(myOriginalMovementSpeed * ((myData.myMovementSpeedModifier + 100.f) / 100.f));

		controller->SetAttackDamage(myOriginalAttackDamage * ((myData.myAttackDamageModifier + 100.f) / 100.f));
		controller->SetAttackRange2(myOriginalAttackRange2 * ((myData.myAttackRange2Modifier + 100.f) / 100.f));
		controller->SetRechargeTime(myOriginalRechargeTime * ((myData.myRechargeTimeModifier + 100.f) / 100.f));

	}
}