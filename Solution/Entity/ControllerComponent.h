#pragma once

namespace Prism
{
	class Terrain;
}

class Behavior;

struct ControllerComponentData;
class ControllerComponent : public Component
{
public:
	ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain);
	~ControllerComponent();

	void Update(float aDelta) override;

	void MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound);
	void AttackTarget(Entity* aEntity, bool aClearCommandQueue, bool& aHasPlayedSound);
	void AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound);
	void Stop(bool& aHasPlayedSound);
	void HoldPosition(bool& aHasPlayedSound);

	const CU::Vector2<float>& GetAcceleration() const;
	float GetVisionRange2() const;
	float GetAttackRange2() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	float GetAttackDamage() const;
	float GetAttackSpeed() const;

	void SetAttackDamage(float aDamage);
	void SetRechargeTime(float aRechargeTime);
	void SetAttackRange2(float aRange2);

	void Reset() override;

private:
	enum class eAction
	{
		IDLE,
		MOVE,
		ATTACK_TARGET,
		RETURN,
		HOLD,
		ATTACK_MOVE
	};

	struct ActionData
	{
		ActionData()
			: myAction(eAction::IDLE)
			, myPosition(-1.f, -1.f)
			, myEntity(nullptr)
		{}
		eAction myAction;
		CU::Vector2<float> myPosition;
		Entity* myEntity;
	};

	void FillCommandList(eAction aAction, bool aClearCommandQueue, Entity* aEntity = nullptr, const CU::Vector2<float>& aTargetPosition = { -1.f, -1.f });
	void DoIdle();
	void DoMove();
	void DoAttackTarget(float aDelta);
	void DoReturn();
	void DoHold();
	void DoAttackMove();


	void AttackTarget();
	void StartNextAction();
	void RefreshPathToAttackTarget();
	void RenderDebugLines() const;
	const CU::Vector2<float>& GetPosition(const ActionData& anActionData) const;
	eColorDebug GetActionColor(eAction aAction) const;
	
	CU::Vector2<float> myAcceleration;
	Behavior* myBehavior;

	CU::Vector2<float> myReturnPosition;

	const Prism::Terrain& myTerrain;
	eOwnerType myOwnerType;
	int myTargetType;

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

	CU::GrowingArray<ActionData> myActions;
	ActionData myCurrentAction;
};

inline const CU::Vector2<float>& ControllerComponent::GetAcceleration() const
{
	return myAcceleration;
}

inline float ControllerComponent::GetVisionRange2() const
{
	return myVisionRange2;
}

inline float ControllerComponent::GetAttackRange2() const
{
	return myAttackRange2;
}

inline eComponentType ControllerComponent::GetTypeStatic()
{
	return eComponentType::CONTROLLER;
}

inline eComponentType ControllerComponent::GetType()
{
	return GetTypeStatic();
}

inline float ControllerComponent::GetAttackDamage() const
{
	return myAttackDamage;
}

inline float ControllerComponent::GetAttackSpeed() const
{
	return myAttackRechargeTime;
}

inline void ControllerComponent::SetAttackDamage(float aDamage)
{
	myAttackDamage = aDamage;
}

inline void ControllerComponent::SetRechargeTime(float aRechargeTime)
{
	myAttackRechargeTime = aRechargeTime;
}

inline void ControllerComponent::SetAttackRange2(float aRange2)
{
	myAttackRange2 = aRange2;
}