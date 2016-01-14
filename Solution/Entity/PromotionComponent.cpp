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
#include "PromotionComponent.h"
#include "PromotionComponentData.h"
#include "TriggerMessage.h"

PromotionComponent::PromotionComponent(Entity& aEntity, PromotionComponentData& aData)
	: Component(aEntity)
	, myKillCount(aData.myKillCount)
{
}

PromotionComponent::~PromotionComponent()
{
}

void PromotionComponent::Update(float aDeltaTime)
{
}