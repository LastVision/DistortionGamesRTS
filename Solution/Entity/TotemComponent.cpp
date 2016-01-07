#include "stdafx.h"

#include <AudioInterface.h>
#include "CollisionComponent.h"
#include "MathHelper.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "MathHelper.h"
#include "PollingStation.h"
#include "PostMaster.h"
#include "SoundComponent.h"
#include "TotemComponent.h"
#include "TotemComponentData.h"
#include "TriggerMessage.h"

TotemComponent::TotemComponent(Entity& aEntity, TotemComponentData& aData)
	: Component(aEntity)
	, myRadius(aData.myRadius)
	, myRadiusSquared(myRadius * myRadius)
	, myHealPerSecond(aData.myHealPerSecond)
	, myCurrentCooldown(0)
	, myOriginalCooldown(aData.myCooldown)
	, myEndTime(aData.myDuration)
	, myDuration(0.f)
	, myUnits(GC::playerUnitCount)
	, myHasReachedTarget(true)
	, myAlpha(0.f)
	, myActive(false)
{
	myOriginalPosition = myEntity.GetOrientation().GetPos();
}

TotemComponent::~TotemComponent()
{
}

void TotemComponent::Update(float aDeltaTime)
{
	myAlpha += aDeltaTime;
	myCurrentCooldown -= aDeltaTime;
	myDuration += aDeltaTime;

	CheckUnitsForRemove(myUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(myEntity.GetOwner()), myUnits);

	if (myDuration >= myEndTime)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Totem"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		myActive = false;
		myEntity.SetPosition(myOriginalPosition);
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			myUnits[i]->GetComponent<HealthComponent>()->SetIsHealing(false);
		}
	}
	else
	{
		if (myHasReachedTarget == true && myActive == true)
		{
			for (int i = 0; i < myUnits.Size(); ++i)
			{
				myUnits[i]->GetComponent<HealthComponent>()->SetIsHealing(true);
			}
		}
	}
	


	if (myActive == true)
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			myUnits[i]->GetComponent<HealthComponent>()->Heal(myHealPerSecond*aDeltaTime);
		}
	}



	if (myHasReachedTarget == false)
	{
		myEntity.SetPosition(CU::Math::Lerp<CU::Vector3f>(myOriginalPosition, myTargetPosition, myAlpha));
	}


	if (myAlpha >= 1.2f && myHasReachedTarget == false)
	{
		myHasReachedTarget = true;
		myActive = true;
		//if (myAlpha >= myOriginalCooldown)
		//{
		//	myActive = false;
		//	myEntity.SetPosition(myOriginalPosition);
		//}
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Totem_HitGround"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Totem"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
	}
}

void TotemComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
{
	for (int i = someUnits.Size() - 1; i >= 0; --i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()) == false
			|| (someUnits[i]->GetState() == eEntityState::DYING && someUnits[i]->GetAlive() == false))
		{
			PostMaster::GetInstance()->SendMessage(
				TriggerMessage(&myEntity, current, TriggerMessage::eTriggerType::EXIT));

			someUnits.RemoveCyclicAtIndex(i);
		}
	}
}

void TotemComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits
	, CU::GrowingArray<Entity*>& someUnitsOut) const
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

void TotemComponent::SetTargetPosition(const CU::Vector3f& aTargetPosition)
{
	if (myCurrentCooldown <= 0.f)
	{
		myTargetPosition = aTargetPosition;
		myOriginalPosition.x = aTargetPosition.x;
		myOriginalPosition.z = aTargetPosition.z;
		myHasReachedTarget = false;
		myAlpha = 0.f;
		myActive = false;
		myCurrentCooldown = myOriginalCooldown;
		myDuration = 0.f;
	}
}