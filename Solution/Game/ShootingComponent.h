#pragma once
#include "Component.h"
#include "Enums.h"
#include <AudioInterface.h>

struct WeaponData
{
	eBulletType myBulletType;
	int myBulletsPerShot;
	int myMultiplier;
	int mySpread;
	int myID;
	float myCoolDownTime;
	float myCurrentTime;
	bool myIsHoming;
	float myHomingTurnRateModifier;
	CU::Vector3<float> myPosition;
	std::string myType;
};

struct WeaponPowerUp
{
	ePowerUpType myPowerUpType;
	float myPowerUpValue;
	//float myPowerUpDuration;
	float myPowerUpCoolDownReducer;
};

class Entity;

struct WeaponDataType;

class ShootingComponent : public Component
{
public:
	ShootingComponent(Entity& aEntity);
	~ShootingComponent();

	void Update(float aDeltaTime) override;

	void ReceiveNote(const ShootNote& aMessage) override;
	void ReceiveNote(const InputNote& aMessage) override;
	void ReceiveNote(const PowerUpNote& aNote) override;
	void ReceiveNote(const EMPNote& aNote) override;

	void AddWeapon(const WeaponDataType& aWeapon);
	void UpgradeWeapon(const WeaponDataType& aWeapon, int aWeaponID, bool anIsIngame = false);

	static eComponentType GetType();

	void SetCurrentWeaponID(int anID);
	const int& GetCurrentWeaponID() const;

	void SetHomingTarget(Entity* aTarget);

	void Reset() override;
	bool HasPowerUp(ePowerUpType aPowerUp);

	const bool& GetFireRatePowerUp() const;
	const bool& GetEMPPowerUp() const;
	const bool& GetHomingPowerUp() const;

	const float& GetFireRatePowerUpDuration() const;
	const float& GetHomingPowerUpDuration() const;

	const int GetWeaponSize() const;

	const float& GetRocketCurrentTime() const;
	const float& GetRocketMaxTime() const;

private:
	void SetActivatePowerUp(ePowerUpType aType, bool aValue);
	void ActivatePowerUp(ePowerUpType aPowerUp); // only for emp
	void AddDuration(ePowerUpType aPowerUp, float aTime);

	CU::GrowingArray<WeaponData, int> myWeapons;

	Entity* myHomingTarget;

	int myCurrentWeaponID;

	bool myHasWeapon;

	CU::GrowingArray<WeaponPowerUp> myPowerUps;

	bool myFireRatePowerUp;
	bool myEMPPowerUp;
	bool myHomingPowerUp;

	bool myHasShotMachinegun;
	bool myHasShotRocket;
	bool myHasSwitchWeapon;

	float myFireRatePowerUpDuration;
	float myEMPPowerUpDuration;
	float myHomingPowerUpDuration;

	float myEMPTime;
	bool myShootSwitch;
};

inline eComponentType ShootingComponent::GetType()
{
	return eComponentType::SHOOTING;
}

inline void ShootingComponent::SetHomingTarget(Entity* aTarget)
{
	if (GetEntity().GetType() == eEntityType::PLAYER && myHomingTarget != aTarget)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_HomingTargetSet", 0);
	}
	myHomingTarget = aTarget;

}

inline eBulletType ConvertToBulletEnum(const std::string& aString)
{
	if (aString == "machinegun1")
	{
		return eBulletType::MACHINGUN_BULLET_LEVEL_1;
	}
	else if (aString == "machinegun2")
	{
		return eBulletType::MACHINGUN_BULLET_LEVEL_2;
	}
	else if (aString == "machinegun3")
	{
		return eBulletType::MACHINGUN_BULLET_LEVEL_3;
	}
	else if (aString == "shotgun1")
	{
		return eBulletType::SHOTGUN_BULLET_LEVEL_1;
	}
	else if (aString == "shotgun2")
	{
		return eBulletType::SHOTGUN_BULLET_LEVEL_2;
	}
	else if (aString == "shotgun3")
	{
		return eBulletType::SHOTGUN_BULLET_LEVEL_3;
	}
	else if (aString == "rocket1")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_1;
	}
	else if (aString == "rocket2")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_2;
	}
	else if (aString == "rocket3")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_3;
	}
	else if (aString == "rocket3")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_3;
	}
	else if (aString == "rocket3")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_3;
	}
	else if (aString == "B_enemy_default")
	{
		return eBulletType::ENEMY_BULLET_DEFAULT;
	}
	else if (aString == "B_enemy_fast")
	{
		return eBulletType::ENEMY_BULLET_FAST;
	}
	else if (aString == "B_enemy_slow")
	{
		return eBulletType::ENEMY_BULLET_SLOW;
	}
	else if (aString == "B_enemy_turret")
	{
		return eBulletType::ENEMY_BULLET_TURRET;
	}
	else if (aString == "B_enemy_homing")
	{
		return eBulletType::ENEMY_BULLET_HOMING;
	}
	return eBulletType::COUNT;
}

inline const bool& ShootingComponent::GetFireRatePowerUp() const
{
	return myFireRatePowerUp;
}

inline const bool& ShootingComponent::GetEMPPowerUp() const
{
	return myEMPPowerUp;
}

inline const bool& ShootingComponent::GetHomingPowerUp() const
{
	return myHomingPowerUp;
}

inline const float& ShootingComponent::GetFireRatePowerUpDuration() const
{
	return myFireRatePowerUpDuration;
}

inline const float& ShootingComponent::GetHomingPowerUpDuration() const
{
	return myHomingPowerUpDuration;
}

inline const int ShootingComponent::GetWeaponSize() const
{
	return myWeapons.Size();
}

inline const int& ShootingComponent::GetCurrentWeaponID() const
{
	return myCurrentWeaponID;
}

inline const float& ShootingComponent::GetRocketCurrentTime() const
{
	return myWeapons[2].myCurrentTime;
}

inline const float& ShootingComponent::GetRocketMaxTime() const
{
	return myWeapons[2].myCoolDownTime;
}