#include "stdafx.h"
#include "CollisionComponent.h"
#include "TriggerComponent.h"
#include "TriggerComponentData.h"
#include <TriggerMessage.h>
#include <Intersection.h>
#include "PollingStation.h"
#include <PostMaster.h>

TriggerComponent::TriggerComponent(Entity& aEntity, TriggerComponentData& aData)
	: Component(aEntity)
	, myType(aData.myType)
	, myPlayerUnits(GC::playerUnitCount)
	, myEnemyUnits(GC::enemyUnitCount)
	, myRadius(aData.myRadius)
	, myRadiusSquared(aData.myRadius * aData.myRadius)
{
}

TriggerComponent::~TriggerComponent()
{
}

void TriggerComponent::Update(float)
{
	CheckUnitsForRemove(myPlayerUnits);
	CheckUnitsForRemove(myEnemyUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myPlayerUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myEnemyUnits);
}

void TriggerComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
{
	for (int i = someUnits.Size() - 1; i >= 0; --i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()) == false)
		{
			PostMaster::GetInstance()->SendMessage(
				TriggerMessage(&myEntity, current, TriggerMessage::eTriggerType::EXIT));

			someUnits.RemoveCyclicAtIndex(i);
		}
	}
}

void TriggerComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits
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