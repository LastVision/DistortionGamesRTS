#include "stdafx.h"
#include "CollisionComponent.h"
#include "CommonHelper.h"
#include "MathHelper.h"
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
	, myOwnershipRatio(0)
	, myGainingPointsOwner(eOwnerType::NEUTRAL)
{
	myOriginalPosition = myEntity.GetOrientation().GetPos();
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

	float height = CU::Math::Remap(myOwnershipRatio, 0.f, 100.f, -5.f, 0.f);
	myEntity.SetPosition({ myOriginalPosition.x, myOriginalPosition.y + height, myOriginalPosition.z });
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

eOwnerType TriggerComponent::ModifyOwnership(eOwnerType anOwner, float aModifyValue)
{
	if (anOwner == eOwnerType::NEUTRAL)
	{
		myOwnershipRatio -= aModifyValue;
	}
	else if (anOwner == myEntity.GetOwner() || myOwnershipRatio <= 0.f)
	{
		myOwnershipRatio += aModifyValue;
		myEntity.SetOwner(anOwner);
	}
	else
	{
		myOwnershipRatio -= aModifyValue;
	}

	myOwnershipRatio = CU::Clip(myOwnershipRatio, 0.f, 100.f);

	if (myOwnershipRatio == 0.f)
	{
		myEntity.SetOwner(eOwnerType::NEUTRAL);
		myGainingPointsOwner = eOwnerType::NEUTRAL;
	}
	else if (myOwnershipRatio == 100.f)
	{
		myEntity.SetOwner(anOwner);
		myGainingPointsOwner = anOwner;
	}

	return myGainingPointsOwner;
}