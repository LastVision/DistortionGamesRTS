#pragma once
#include "Component.h"
#include <Vector.h>
#include <GrowingArray.h>

namespace Prism
{
	class Terrain;
}

class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, float aMovementSpeed, Prism::Terrain& aTerrain);
	~MovementComponent();

	void Update(float aDeltaTime) override;
	void SetWayPoints(const CU::GrowingArray<CU::Vector3<float>>& someWayPoints);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	void MoveTo(CU::Vector3<float> aPointToMoveTo);

	float myMovementSpeed;
	CU::Vector3<float> myTargetPosition;
	CU::Vector3<float> myDirection;
	CU::GrowingArray<CU::Vector3<float>> myWayPoints;
	int myCurrentWayPoint;
	Prism::Terrain& myTerrain;
};

inline eComponentType MovementComponent::GetTypeStatic()
{
	return eComponentType::MOVEMENT;
}

inline eComponentType MovementComponent::GetType()
{
	return GetTypeStatic();
}
