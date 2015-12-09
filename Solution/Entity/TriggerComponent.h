#pragma once

#include "Component.h"
#include <LineSegment3D.h>
#include <Sphere.h>

struct TriggerComponentData;

class TriggerComponent : public Component
{
public:
	TriggerComponent(Entity& aEntity, TriggerComponentData& aData);
	~TriggerComponent();

	void Update(float aDelta) override;
	bool Collide(const CU::Intersection::LineSegment3D& aLine) const;
	bool Collide(const CU::Vector3<float>& aPosition) const;

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();

private:
	float myRadius;
	CU::Intersection::Sphere mySphere;
};

inline eComponentType TriggerComponent::GetTypeStatic()
{
	return eComponentType::TRIGGER;
}

inline eComponentType TriggerComponent::GetType()
{
	return GetTypeStatic();
}
