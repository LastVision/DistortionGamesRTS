#include "stdafx.h"
#include "CollisionComponent.h"
#include "CollisionComponentData.h"
#include "Entity.h"
#include <Intersection.h>

CollisionComponent::CollisionComponent(Entity& aEntity, CollisionComponentData& aData)
	: Component(aEntity)
{
	mySphere.myRadius = aData.myRadius;
	mySphere.myRadiusSquared = aData.myRadius * aData.myRadius;
	mySphere.myCenterPosition = myEntity.GetOrientation().GetPos();
}


CollisionComponent::~CollisionComponent()
{
}

void CollisionComponent::Update(float)
{
	mySphere.myCenterPosition = myEntity.GetOrientation().GetPos();
}

bool CollisionComponent::Collide(const CU::Intersection::LineSegment3D& aLine) const
{
	CU::Vector3<float> notUsed;
	return CU::Intersection::LineVsSphere(aLine, mySphere, notUsed);
}