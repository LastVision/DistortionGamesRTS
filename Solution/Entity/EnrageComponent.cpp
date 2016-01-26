#include "stdafx.h"
#include "ActorComponent.h"
#include <AudioInterface.h>
#include "ControllerComponent.h"
#include "CollisionComponent.h"
#include "EnrageComponent.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "Postmaster.h"
#include "PollingStation.h"
#include "PromotionComponent.h"
#include "SoundComponent.h"
#include <TriggerMessage.h>

EnrageComponent::EnrageComponent(Entity& anEntity, EnrageComponentData& aData)
	: Component(anEntity)
	, myCurrentCooldown(0.f)
	, myCurrentDuration(0.f)
	, myIsActive(false)
	, myData(aData)
	, myUnits(8)
	, myRadius(aData.myRadius)
	, myUnitActivationCount(aData.myActivationUnitCount)
{
}

EnrageComponent::~EnrageComponent()
{
}

void EnrageComponent::Update(float aDeltaTime)
{
	if (myEntity.GetComponent<PromotionComponent>()->GetPromoted() == false)
	{
		return;
	}
	myCurrentCooldown -= aDeltaTime;

	CheckUnitsForRemove(myUnits);
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myUnits);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myUnits);
	}
	else if (myEntity.GetOwner() == eOwnerType::ENEMY)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myUnits);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myUnits);
	}
	else if (myEntity.GetOwner() == eOwnerType::NEUTRAL)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myUnits);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myUnits);
	}

	if (myUnits.Size() >= myUnitActivationCount && myIsActive == false)
	{
		Activate();
	}

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

void EnrageComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const
{
	for (int i = 0; i < someUnits.Size(); ++i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()))
		{
			if (someUnitsOut.Find(current) < 0)
			{
				PostMaster::GetInstance()->SendMessage(
					TriggerMessage(&myEntity, current, TriggerMessage::eTriggerType::ENTER));

				someUnitsOut.Add(current);
			}
		}
	}
}

void EnrageComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
{
	for (int i = someUnits.Size() - 1; i >= 0; --i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()) == false
			|| (someUnits[i]->GetState() == eEntityState::DIE && someUnits[i]->GetAlive() == false))
		{
			PostMaster::GetInstance()->SendMessage(
				TriggerMessage(&myEntity, current, TriggerMessage::eTriggerType::EXIT));

			someUnits.RemoveCyclicAtIndex(i);
		}
	}
}