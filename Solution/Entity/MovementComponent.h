#pragma once
#include "Component.h"
#include <Vector.h>
#include <GrowingArray.h>

namespace Prism
{
	class Terrain;
}


struct MovementComponentData;

class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, MovementComponentData& aComponentData, const Prism::Terrain& aTerrain);
	~MovementComponent();

	void Update(float aDeltaTime) override;
	void AddWayPoints(const CU::GrowingArray<CU::Vector3<float>>& someWayPoints, bool aRemovePreviousPoints);
	void AddWayPoint(const CU::Vector3<float>& aWayPoint, bool aRemovePreviousPoints);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	void operator=(MovementComponent&) = delete;

	void MoveTo(CU::Vector3<float> aPointToMoveTo);

	float myMovementSpeed;
	CU::Vector3<float> myTargetPosition;
	CU::Vector3<float> myDirection;
	CU::GrowingArray<CU::Vector3<float>> myWayPoints;
	int myCurrentWayPoint;
	const Prism::Terrain& myTerrain;
};

inline eComponentType MovementComponent::GetTypeStatic()
{
	return eComponentType::MOVEMENT;
}

inline eComponentType MovementComponent::GetType()
{
	return GetTypeStatic();
}
