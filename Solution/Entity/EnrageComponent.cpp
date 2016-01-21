#include "stdafx.h"
#include "ActorComponent.h"
#include <AudioInterface.h>
#include "ControllerComponent.h"
#include "EnrageComponent.h"
#include "HealthComponent.h"
#include "SoundComponent.h"

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
			ActorComponent* actor = myEntity.GetComponent<ActorComponent>();

			health->SetArmor(myOriginalArmor);
			myEntity.SetMaxSpeed(myOriginalMovementSpeed);
			actor->SetAttackDamage(myOriginalAttackDamage);
			actor->SetAttackRange2(myOriginalAttackRange2);
			actor->SetRechargeTime(myOriginalRechargeTime);

		}
	}
}

void EnrageComponent::Activate()
{
	if (myCurrentCooldown <= 0.f)
	{
		HealthComponent* health = myEntity.GetComponent<HealthComponent>();
		ActorComponent* actor = myEntity.GetComponent<ActorComponent>();


		myIsActive = true;
		myCurrentDuration = myData.myDuration;
		myCurrentCooldown = myData.myCooldown;

		myOriginalArmor = health->GetArmor();
		myOriginalMovementSpeed = myEntity.GetMaxSpeed();
		myOriginalAttackDamage = actor->GetAttackDamage();
		myOriginalAttackRange2 = actor->GetAttackRange2();
		myOriginalRechargeTime = actor->GetAttackSpeed();


		health->TakeDamageAndCheckSurvive((health->GetMaxHealth() * (myData.myHealthModifier / 100.f)) + myOriginalArmor);
		health->SetArmor(myOriginalArmor * ((myData.myArmorModifier + 100.f) / 100.f));
		myEntity.SetMaxSpeed(myOriginalMovementSpeed * ((myData.myMovementSpeedModifier + 100.f) / 100.f));

		actor->SetAttackDamage(myOriginalAttackDamage * ((myData.myAttackDamageModifier + 100.f) / 100.f));
		actor->SetAttackRange2(myOriginalAttackRange2 * ((myData.myAttackRange2Modifier + 100.f) / 100.f));
		actor->SetRechargeTime(myOriginalRechargeTime * ((myData.myRechargeTimeModifier + 100.f) / 100.f));

		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Enrage"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());

	}
}