#include "stdafx.h"

#include "CollisionComponent.h"
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "Postmaster.h"
#include "PollingStation.h"
#include "TriggerMessage.h"

GrenadeComponent::GrenadeComponent(Entity& anEntity, GrenadeComponentData& aGrenadeData)
	: Component(anEntity)
	, myOriginalDelay(aGrenadeData.myDelay)
	, myOriginalCooldown(aGrenadeData.myCooldown)
	, myRange2(aGrenadeData.myRange * aGrenadeData.myRange)
	, myRadius(aGrenadeData.myRadius)
	, myDamage(aGrenadeData.myDamage)
	, myDelay(0)
	, myUnits(GC::playerUnitCount)
{
}


GrenadeComponent::~GrenadeComponent()
{
}

void GrenadeComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
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

void GrenadeComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits
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

void GrenadeComponent::Update(float aDeltaTime)
{
	CheckUnitsForRemove(myUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(myEntity.GetOwner()), myUnits);

	myCurrentCooldown -= aDeltaTime;
	myDelay -= aDeltaTime;
	if (myHasExploded == false)
	{
		if (myDelay <= 0.f)
		{
			Explosion();
		}
	}
}

void GrenadeComponent::ThrowGrenade(const CU::Vector3f& aTargetPosition)
{
	myDelay = myOriginalDelay;
	myPosition = aTargetPosition;
	myCurrentCooldown = myOriginalCooldown;
	myHasExploded = false;
}

float GrenadeComponent::GetCooldown() const
{
	return myCurrentCooldown;
}

void GrenadeComponent::Explosion()
{
	CU::Vector2<float> my2DPosition = { myPosition.x, myPosition.z };
	//sendmessagetoparticlemanagerwithposition
	if (myUnits.Size() > 0)
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			float length2 = CU::Length2(myUnits[i]->GetPosition() - my2DPosition);
			myUnits[i]->GetComponent<HealthComponent>()->TakeDamage(myDamage);
		}
	}
	myHasExploded = true;
}