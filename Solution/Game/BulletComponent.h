#pragma once
#include "Component.h"
#include "Enums.h"

class BulletComponent : public Component
{
public:
	BulletComponent(Entity& aEntity);
	
	void Update(float aDeltaTime) override;
	void Init(float aMaxTime, int aDamage, float aDamageRadius, eBulletType aType);
	void SetOwner(eEntityType aOwner);

	void ReceiveNote(const CollisionNote& aNote) override;

	static eComponentType GetType();

	void SetMaxLifeTime(float aMaxTime);
	void SetActive(bool aActive);
	bool GetActive() const;

	float GetCurrentLifeTime() const;
	eBulletType GetBulletType();
private:
	eBulletType myType;
	eEntityType myOwner;
	float myCurrentLifeTime;
	float myMaxLifeTime;
	float myDamageRadius;
	bool myActive;
	int myDamage;
};

inline eComponentType BulletComponent::GetType()
{
	return eComponentType::BULLET;
}

inline void BulletComponent::SetMaxLifeTime(float aMaxTime)
{
	myMaxLifeTime = aMaxTime;
}

inline bool BulletComponent::GetActive() const
{
	return myActive;
}

inline float BulletComponent::GetCurrentLifeTime() const
{
	return myCurrentLifeTime;
}

inline eBulletType BulletComponent::GetBulletType()
{
	return myType;
}