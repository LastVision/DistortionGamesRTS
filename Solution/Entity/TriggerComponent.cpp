#include "stdafx.h"
#include "CollisionComponent.h"
#include "CommonHelper.h"
#include "MathHelper.h"
#include "TriggerComponent.h"
#include <TriggerMessage.h>
#include <Intersection.h>
#include "PollingStation.h"
#include <PostMaster.h>
#include <MinimapEventMessage.h>

TriggerComponent::TriggerComponent(Entity& aEntity, TriggerComponentData& aData)
	: Component(aEntity)
	, myType(aData.myType)
	, myPlayerUnits(GC::playerUnitCount)
	, myEnemyUnits(GC::enemyUnitCount)
	, myNeutralUnits(32)
	, myRadius(aData.myRadius)
	, myRadiusSquared(aData.myRadius * aData.myRadius)
	, myOwnershipRatio(0)
	, myGainingPointsOwner(eOwnerType::NEUTRAL)
	, myHasSentEventMessage(false)
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
	CheckUnitsForRemove(myNeutralUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myPlayerUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myEnemyUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myNeutralUnits);

	float height = CU::Math::Remap(myOwnershipRatio, 0.f, 100.f, -5.f, 0.f);
	myEntity.SetPosition({ myOriginalPosition.x, myOriginalPosition.y + height, myOriginalPosition.z });
}

void TriggerComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
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
		if (anOwner == eOwnerType::ENEMY)
		{
			if (myHasSentEventMessage == false)
			{
				if (myType == eTriggerType::RESOURCE)
				{
					PostMaster::GetInstance()->SendMessage<MinimapEventMessage>(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eRESOURCE_POINT));
				}
				else if (myType == eTriggerType::VICTORY)
				{
					PostMaster::GetInstance()->SendMessage<MinimapEventMessage>(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eVICTORY_POINT));
				}
				else
				{
					DL_ASSERT("INVALID TYPE IN TRIGGER");
				}
				myHasSentEventMessage = true;
			}
		}
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
		myHasSentEventMessage = false;
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