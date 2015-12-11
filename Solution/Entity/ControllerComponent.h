#pragma once

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

	void MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue);
	void AttackTarget(const Entity* aEntity, bool aClearCommandQueue);
	void AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue);
	void Stop();
	void HoldPosition();

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
		RETURN,
		HOLD_POSITION,
		MOVE_ATTACK
	};

	struct ActionData
	{
		eAction myAction;
		CU::Vector3<float> myPosition;
	};

	void FillCommandList(const CU::Vector3<float>& aTargetPosition, eAction aAction, bool aClearCommandQueue);
	void DoMoveAction(const CU::Vector3<float>& aTargetPosition);
	void DoAttackAction();
	void AttackTarget();
	void StartNextAction();
	void RenderDebugLines() const;
	eColorDebug GetActionColor(eAction aAction) const;
	
	Entity* myAttackTarget;
	ControllerData myData;

	CU::Vector3<float> myReturnPosition;

	const Prism::Terrain& myTerrain;
	eOwnerType myOwnerType;
	eOwnerType myTargetType;

	float myVisionRange;
	float myAttackRange;
	float myChaseDistance;


	CU::GrowingArray<ActionData> myActions;
	ActionData myCurrentAction;
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