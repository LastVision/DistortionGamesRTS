#pragma once

struct WeaponDataType
{
	std::string myBulletType;
	std::string myType;

	int mySpread;
	int myBulletsPerShot;

	float myCoolDownTime;
	float myCurrentTime;
	CU::Vector3<float> myPosition;
	bool myIsHoming;
	float myHomingTurnRateModifier;
};

struct ProjectileDataType
{
	std::string myType;
	std::string myEntityType;

	int myMaxBullet;

	float mySpeed;
	float myCollision;
};

class WeaponFactory
{
public:
	void LoadProjectiles(const std::string& aRootFilePath);
	void LoadWeapons(const std::string& aRootFilePath);

	ProjectileDataType GetProjectile(const std::string& aProjectileType);
	WeaponDataType GetWeapon(const std::string& aWeaponName);

	void ReloadWeapon(const std::string&);
private:
	void LoadProjectile(const std::string& aProjectileFilePath);
	void LoadWeapon(const std::string& aWeaponFilePath);

	std::unordered_map<std::string, ProjectileDataType> myProjectileTypes;
	std::unordered_map<std::string, WeaponDataType> myWeaponsTypes;
};

