#pragma once
#include "Component.h"

namespace Prism
{
	class Terrain;
}

class Behavior;

struct ActorComponentData;

class ActorComponent : public Component
{
public:
	ActorComponent(Entity& aEntity, ActorComponentData& aData, const Prism::Terrain& aTerrain
		, const EntityCommandData& aCurrentCommand);
	~ActorComponent();

	void Reset() override;

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;


	void LookInDirection(const CU::Vector2<float>& aDirection, float aDelta);
	void LookAtPoint(const CU::Vector2<float>& aPoint, float aDelta);

	float GetVisionRange() const;
	float GetVisionRange2() const;
	float GetAttackRange2() const;


	float GetAttackDamage() const;
	float GetAttackSpeed() const;

	void SetAttackDamage(float aDamage);
	void SetRechargeTime(float aRechargeTime);
	void SetAttackRange2(float aRange2);

	const float& GetRangerCooldown() const;
	const float& GetRangerMaxCooldown() const;

private:

	void DoStop(float aDelta);
	void DoAttackTarget(float aDelta);
	void DoAttackMove(float aDelta);
	void DoMove(float aDelta);
	void DoHoldPosition(float aDelta);

	void StandStill();

	void CheckAnimationComplete();
	Entity* FindAttackTarget();
	
	void AttackTarget(Entity* aTarget, float aDelta);

	const Prism::Terrain& myTerrain;
	CU::Vector2<float> myAcceleration;

	Behavior* myBehavior;
	const EntityCommandData& myCurrentCommand;
	EntityCommandData myPreviousCommand;

	int myTargetType;

	bool myIsDone;
	bool myIsReturning;

	float myVisionRange;
	float myVisionRange2;
	float myAttackRange2;
	float myAttackDamage;
	float myAttackRechargeTime;
	float myAttackTimer;
	float myChaseDistance2;
	float myChaseDistanceNeutral2;
	float myAttackTargetPathRefreshTime;
	float myCurrentAttackTargetPathRefreshTime;

	float myRangerOneShotTimer;
	float myRangerOneShotCooldown;
};

inline eComponentType ActorComponent::GetTypeStatic()
{
	return eComponentType::ACTOR;
}

inline eComponentType ActorComponent::GetType()
{
	return GetTypeStatic();
}

inline float ActorComponent::GetVisionRange() const
{
	return myVisionRange;
}

inline float ActorComponent::GetVisionRange2() const
{
	return myVisionRange2;
}

inline float ActorComponent::GetAttackRange2() const
{
	return myAttackRange2;
}

inline float ActorComponent::GetAttackDamage() const
{
	return myAttackDamage;
}

inline float ActorComponent::GetAttackSpeed() const
{
	return myAttackRechargeTime;
}

inline void ActorComponent::SetAttackDamage(float aDamage)
{
	myAttackDamage = static_cast<float>(static_cast<int>(aDamage));
}

inline void ActorComponent::SetRechargeTime(float aRechargeTime)
{
	myAttackRechargeTime = aRechargeTime;
}

inline void ActorComponent::SetAttackRange2(float aRange2)
{
	myAttackRange2 = aRange2;
}

inline const float& ActorComponent::GetRangerCooldown() const
{
	return myRangerOneShotTimer;
}

inline const float& ActorComponent::GetRangerMaxCooldown() const
{
	return myRangerOneShotCooldown;
}