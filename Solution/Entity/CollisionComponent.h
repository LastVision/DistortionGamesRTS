#pragma once

#include "Component.h"
#include <LineSegment3D.h>
#include <Sphere.h>

struct CollisionComponentData;

class CollisionComponent : public Component
{
public:
	CollisionComponent(Entity& aEntity, CollisionComponentData& aData);
	~CollisionComponent();

	void Update(float aDelta) override;
	bool Collide(const CU::Intersection::LineSegment3D& aLine) const;
	bool Collide(const CU::Vector3<float>& aPosition) const;

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();

private:
	float myRadius;
	CU::Intersection::Sphere mySphere;
};

inline eComponentType CollisionComponent::GetTypeStatic()
{
	return eComponentType::COLLISION;
}

inline eComponentType CollisionComponent::GetType()
{
	return GetTypeStatic();
}
