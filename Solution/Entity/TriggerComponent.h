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

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	eOwnerType ModifyOwnership(eOwnerType anOwner, float aModifyValue);

private:
	void operator=(TriggerComponent&) = delete;

	void CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const;
	void CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const;

	const float myRadius;
	const float myRadiusSquared;

	float myOwnershipRatio;
	CU::Vector3f myOriginalPosition;

	eOwnerType myGainingPointsOwner;
	const eTriggerType myType;
	CU::GrowingArray<Entity*> myPlayerUnits;
	CU::GrowingArray<Entity*> myEnemyUnits;
};

inline eComponentType TriggerComponent::GetTypeStatic()
{
	return eComponentType::TRIGGER;
}

inline eComponentType TriggerComponent::GetType()
{
	return GetTypeStatic();
}
