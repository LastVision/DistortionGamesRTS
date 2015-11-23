#pragma once
#include "ControllerComponent.h"

namespace CU
{
	class InputWrapper;
}

class InputComponent : public ControllerComponent
{
public:
	InputComponent(Entity& aEntity);

	void Init(CU::InputWrapper& aInputWrapper);
	void Update(float aDeltaTime) override;

	const CU::Matrix44<float>& GetSkyOrientation() const;

	void SetSkyPosition();

	static eComponentType GetType();

	void Reset() override;

private:
	void operator=(const InputComponent&) = delete;
	void ReadXML(const std::string& aFile);

	void Roll(float aDeltaTime);
	void ToggleCameraLock();

	void UpdateMovement(const float& aDelta);
	void UpdateSteering(const float& aDelta);

	CU::InputWrapper* myInputWrapper;


	CU::Vector2<float> mySteering;
	float mySteeringModifier;
	float mySteeringDeltaClip;
	float mySteeringDeacceleration;
	float mySteeringDeaccelerationLowerLimit;
	float myRollSpeed;
	float myMaxRollSpeed;
	float myRollAcceleration;
	float myRollDeacceleration;
	float myMaxMovementSpeed;
	float myMinMovementSpeed;
	float myMaxSteeringSpeed;
	float myAcceleration;
	float myBoostSteeringLimiter;

	float myBoostAcceleration;
	float myBoostDeacceleration;
	float myCurrentBoostValue;
	float myMaxBoostValue;
	float myCurrentBoostCooldown;
	float myMaxBoostCooldown;
	float myWeaponRotationModifier;

	bool myCameraIsLocked;
	bool myBoost;
	bool myCanChangeWeapon;

	CU::Matrix44<float> mySkyOrientation;
};


inline eComponentType InputComponent::GetType()
{
	return eComponentType::INPUT;
}

inline const CU::Matrix44<float>& InputComponent::GetSkyOrientation() const
{
	return mySkyOrientation;
}