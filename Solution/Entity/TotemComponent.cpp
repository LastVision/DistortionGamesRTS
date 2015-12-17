#include "stdafx.h"

#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "MathHelper.h"
#include "PollingStation.h"
#include "PostMaster.h"
#include "TotemComponent.h"
#include "TriggerMessage.h"

TotemComponent::TotemComponent(Entity& aEntity, eOwnerType anOwner, float aRadius, float aHealPerSecond)
	: Component(aEntity)
	, myOwner(anOwner)
	, myRadius(aRadius)
	, myRadiusSquared(myRadius * myRadius)
	, myHealPerSecond(aHealPerSecond)
{
}


TotemComponent::~TotemComponent()
{
}

void TotemComponent::Update(float aDeltaTime)
{
	CheckUnitsForRemove(myUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(myOwner), myUnits);

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		myUnits[i]->GetComponent<HealthComponent>()->Heal(myHealPerSecond*aDeltaTime);
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