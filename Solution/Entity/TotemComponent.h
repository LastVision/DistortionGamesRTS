#pragma once
#include "Component.h"
class TotemComponent :
	public Component
{
public:
	TotemComponent(Entity& aEntity, TotemComponentData& aData);
	~TotemComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();
	eOwnerType ModifyOwnership(eOwnerType anOwner, float aModifyValue);
	void SetTargetPosition(const CU::Vector3f& aTargetPosition);
	float GetCurrentCooldown();
private:
	void operator=(TotemComponent&) = delete;

	void CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const;
	void CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const;

	const float myRadius;
	const float myRadiusSquared;
	const float myHealPerSecond;
	const float myOriginalCooldown;
	
	const float myEndTime;
	float myDuration;
	float myCurrentCooldown;


	float myAlpha;
	bool myHasReachedTarget;

	bool myActive;


	CU::Vector3f myOriginalPosition;
	CU::Vector3f myTargetPosition;

	CU::GrowingArray<Entity*> myUnits;
};

inline eComponentType TotemComponent::GetTypeStatic()
{
	return eComponentType::TOTEM;
}

inline eComponentType TotemComponent::GetType()
{
	return GetTypeStatic();
}

inline float TotemComponent::GetCurrentCooldown()
{
	return myCurrentCooldown;
}