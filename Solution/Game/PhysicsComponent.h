#pragma once
#include "Component.h"

class PhysicsComponent : public Component
{
public:
	PhysicsComponent(Entity& aEntity);

	void Init(const CU::Matrix44<float>& anOrientation, const CU::Vector3<float>& aVelocity = { 0.f, 0.f, 0.f }, int aWeight = 1);
	void Init(int aWeight = 1, const CU::Vector3<float>& aVelocity = { 0.f, 0.f, 0.f });
	void Update(float aDeltaTime);

	static eComponentType GetType();

	int GetWeight() const;

	void SetVelocity(const CU::Vector3<float>& aVelocity);
	const CU::Vector3<float>& GetVelocity() const;
	float GetSpeed() const;

	void Accelerate(float anAcceleration);

	void Reset() override;
	void BounceOff(Entity& anOtherEntity);
	void Bounce();

private:
	float mySpeed;
	CU::Vector3<float> myVelocity;
	int myWeight;
	CU::Matrix44<float> myPreviousOrientation;
	CU::Matrix44<float> myPreviousOrientation2;
};

inline eComponentType PhysicsComponent::GetType()
{
	return eComponentType::PHYSICS;
}

inline int PhysicsComponent::GetWeight() const
{
	return myWeight;
}

inline void PhysicsComponent::SetVelocity(const CU::Vector3<float>& aVelocity)
{
	myVelocity = aVelocity;
	mySpeed = CU::Length(myVelocity);
}

inline const CU::Vector3<float>& PhysicsComponent::GetVelocity() const
{
	return myVelocity;
}

inline float PhysicsComponent::GetSpeed() const
{
	return mySpeed;
}