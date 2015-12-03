#pragma once
#include "Component.h"
#include <GrowingArray.h>
#include <Vector.h>

namespace Prism
{
	class Terrain;
}

struct ControllerComponentData;
class ControllerComponent : public Component
{
public:
	struct ControllerData
	{
		CU::Vector3<float> myDirection;
	};

	ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain);
	~ControllerComponent();

	void Update(float aDelta) override;

	void MoveTo(const CU::Vector3<float>& aPosition, bool aClearPreviousPoints);
	void AttackMove(const CU::Vector3<float>& aPosition);
	void Attack(Entity* aTarget);

	void Spawn(const CU::Vector3f& aPosition);

	const ControllerData& GetControllerData() const;
	float GetVisionRange() const;
	float GetAttackRange() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override; 

private:
	enum class eAction
	{
		IDLE,
		MOVE,
		ATTACK,
		ATTACK_MOVE,
		CHASE,
	};

	void DoMoveAction();
	void DoMoveAction(const CU::Vector3<float>& aTargetPosition);
	void DoAttackAction();
	CU::Vector3<float> GetNextWayPoint();

	eAction myCurrentAction;
	Entity* myAttackTarget;
	ControllerData myData;
	CU::GrowingArray<CU::Vector3<float>> myWayPoints;
	CU::Vector3<float> myCurrentWayPoint;

	CU::Vector3<float> myMoveTarget;
	CU::Vector3<float> myChaseOrigin;

	const Prism::Terrain& myTerrain;

	float myVisionRange;
	float myAttackRange;
	float myChaseDistance;
};

inline const ControllerComponent::ControllerData& ControllerComponent::GetControllerData() const
{
	return myData;
}

inline float ControllerComponent::GetVisionRange() const
{
	return myVisionRange;
}

inline float ControllerComponent::GetAttackRange() const
{
	return myAttackRange;
}

inline eComponentType ControllerComponent::GetTypeStatic()
{
	return eComponentType::CONTROLLER;
}

inline eComponentType ControllerComponent::GetType()
{
	return GetTypeStatic();
}