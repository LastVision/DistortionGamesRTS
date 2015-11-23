#pragma once
#include "Enums.h"
#include "Message.h"

class BulletMessage : public Message
{
public:
	BulletMessage(eBulletType aType, const CU::Matrix44<float>& anOrientation, eEntityType aEntityType
		, const CU::Vector3<float>& aEnitityVelocity, const CU::Vector3<float>& aDirection
		, Entity* aHomingTarget = nullptr, float aHomingTurnRateModifier = 1337.f); // used for timed homing powerup

	eBulletType GetBulletType() const;
	eEntityType GetEntityType() const;
	const CU::Matrix44<float>& GetOrientation() const;
	const CU::Vector3<float>& GetEntityVelocity() const;
	const CU::Vector3<float>& GetDirection() const;
	Entity* GetHomingTarget() const;
	float GetHomingTurnRateModifier() const;
private:
	void operator=(BulletMessage&) = delete;
	CU::Matrix44<float> myOrientation;
	const eBulletType myType;
	const eEntityType myEntityType;
	const CU::Vector3<float> myEntityVelocity;
	const CU::Vector3<float> myDirection;
	Entity* myHomingTarget;
	float myHomingTurnRateModifier;
};

inline eBulletType BulletMessage::GetBulletType() const
{
	return myType;
}

inline eEntityType BulletMessage::GetEntityType() const
{
	return myEntityType;
}

inline const CU::Matrix44<float>& BulletMessage::GetOrientation() const
{
	return myOrientation;
}

inline const CU::Vector3<float>& BulletMessage::GetEntityVelocity() const
{
	return myEntityVelocity;
}

inline const CU::Vector3<float>& BulletMessage::GetDirection() const
{
	return myDirection;
}

inline Entity* BulletMessage::GetHomingTarget() const
{
	return myHomingTarget;
}

inline float BulletMessage::GetHomingTurnRateModifier() const
{
	return myHomingTurnRateModifier;
}