#include "stdafx.h"
#include "TriggerComponent.h"
#include "TriggerComponentData.h"
#include "Entity.h"
#include <Intersection.h>

TriggerComponent::TriggerComponent(Entity& aEntity, TriggerComponentData& aData)
	: Component(aEntity)
{
	mySphere.myRadius = aData.myRadius;
	mySphere.myRadiusSquared = aData.myRadius * aData.myRadius;
	mySphere.myCenterPosition = myEntity.GetOrientation().GetPos();
}


TriggerComponent::~TriggerComponent()
{
}

void TriggerComponent::Update(float)
{
	mySphere.myCenterPosition = myEntity.GetOrientation().GetPos();
}

bool TriggerComponent::Collide(const CU::Intersection::LineSegment3D& aLine) const
{
	CU::Vector3<float> notUsed;
	return CU::Intersection::LineVsSphere(aLine, mySphere, notUsed);
}

bool TriggerComponent::Collide(const CU::Vector3<float>& aPosition) const
{
	return CU::Intersection::PointInsideSphere(mySphere, aPosition);
}