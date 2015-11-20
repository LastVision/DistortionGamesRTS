#include "stdafx.h"

#include <FileWatcher.h>
#include "GameStateMessage.h"
#include "PostMaster.h"
#include "WeaponFactory.h"
#include <XMLReader.h>

void WeaponFactory::LoadProjectiles(const std::string& aRootFilePath)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aRootFilePath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");

	WATCH_FILE(aRootFilePath, WeaponFactory::ReloadWeapon);

	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string projectilePath = "";
		rootDocument.ForceReadAttribute(e, "src", projectilePath);
		if (projectilePath != "")
		{
			LoadProjectile(projectilePath);
			WATCH_FILE(projectilePath, WeaponFactory::ReloadWeapon);
		}
	}

	rootDocument.CloseDocument();
}

void WeaponFactory::LoadWeapons(const std::string& aRootFilePath)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aRootFilePath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");

	WATCH_FILE(aRootFilePath, WeaponFactory::ReloadWeapon);

	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string weaponPath = "";
		rootDocument.ForceReadAttribute(e, "src", weaponPath);
		if (weaponPath != "")
		{
			LoadWeapon(weaponPath);
			WATCH_FILE(weaponPath, WeaponFactory::ReloadWeapon);
		}
	}

	rootDocument.CloseDocument();
}

ProjectileDataType WeaponFactory::GetProjectile(const std::string& aProjectileType)
{
	auto it = myProjectileTypes.find(aProjectileType);
	return it->second;
}

WeaponDataType WeaponFactory::GetWeapon(const std::string& aWeaponName)
{
	auto it = myWeaponsTypes.find(aWeaponName);
	if (myWeaponsTypes.find(aWeaponName) == myWeaponsTypes.end())
	{
		std::string errorMessage = "[WeaponFactory]: Could not find the weapon " + aWeaponName + ".";
		DL_ASSERT(errorMessage.c_str());
	}
	return it->second;
}

void WeaponFactory::LoadProjectile(const std::string& aProjectileFilePath)
{
	XMLReader projectileDocument;
	projectileDocument.OpenDocument(aProjectileFilePath);
	tinyxml2::XMLElement* projectileElement;
	tinyxml2::XMLElement* rootElement = projectileDocument.FindFirstChild("root");
	if (rootElement == nullptr)
	{
		projectileElement = projectileDocument.FindFirstChild("Projectile");
	} 
	else
	{
		projectileElement = projectileDocument.FindFirstChild(rootElement, "Projectile");
	}
	
	ProjectileDataType projectileType;
	projectileType.myCollision = -1.f;

	projectileDocument.ForceReadAttribute(projectileElement, "type", projectileType.myType);

	for (tinyxml2::XMLElement* e = projectileDocument.FindFirstChild(projectileElement); e != nullptr;
		e = projectileDocument.FindNextElement(e))
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Entity").c_str()) == 0)
		{
			if (e->Attribute("type") != nullptr) 
			{
				projectileDocument.ForceReadAttribute(e, "type", projectileType.myEntityType);
			}
			else 
			{
				projectileDocument.ForceReadAttribute(e, "name", projectileType.myEntityType);
			}
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("maxAmount").c_str()) == 0)
		{
			projectileDocument.ForceReadAttribute(e, "value", projectileType.myMaxBullet);
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("speed").c_str()) == 0)
		{
			projectileDocument.ForceReadAttribute(e, "value", projectileType.mySpeed);
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("collision").c_str()) == 0)
		{
			projectileDocument.ForceReadAttribute(e, "value", projectileType.myCollision);
		}
	}

	myProjectileTypes.insert(std::pair<std::string, ProjectileDataType>(projectileType.myType, projectileType));

	projectileDocument.CloseDocument();
}

void WeaponFactory::LoadWeapon(const std::string& aWeaponFilePath)
{
	XMLReader weaponDocument;
	weaponDocument.OpenDocument(aWeaponFilePath);
	tinyxml2::XMLElement* weaponElement;
	tinyxml2::XMLElement* rootElement = weaponDocument.FindFirstChild("root");
	if (rootElement == nullptr)
	{
		weaponElement = weaponDocument.FindFirstChild("Weapon");
	}
	else
	{
		weaponElement = weaponDocument.FindFirstChild(rootElement, "Weapon");
	}

	WeaponDataType weaponDataType;
	weaponDataType.myBulletsPerShot = 1;
	weaponDataType.myIsHoming = false;

	weaponDocument.ForceReadAttribute(weaponElement, "name", weaponDataType.myType);

	for (tinyxml2::XMLElement* e = weaponDocument.FindFirstChild(weaponElement); e != nullptr;
		e = weaponDocument.FindNextElement(e))
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("cooldown").c_str()) == 0)
		{
			weaponDocument.ForceReadAttribute(e, "value", weaponDataType.myCoolDownTime);
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("spread").c_str()) == 0)
		{
			weaponDocument.ForceReadAttribute(e, "value", weaponDataType.mySpread);
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("bulletsPerShot").c_str()) == 0)
		{
			weaponDocument.ForceReadAttribute(e, "value", weaponDataType.myBulletsPerShot);
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("position").c_str()) == 0)
		{
			weaponDocument.ForceReadAttribute(e, "x", weaponDataType.myPosition.myX);
			weaponDocument.ForceReadAttribute(e, "y", weaponDataType.myPosition.myY);
			weaponDocument.ForceReadAttribute(e, "z", weaponDataType.myPosition.myZ);
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("bullet").c_str()) == 0)
		{
			weaponDocument.ForceReadAttribute(e, "type", weaponDataType.myBulletType);
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("homing").c_str()) == 0)
		{
			weaponDocument.ForceReadAttribute(e, "bool", weaponDataType.myIsHoming);
			weaponDocument.ForceReadAttribute(e, "turnRate", weaponDataType.myHomingTurnRateModifier);
		}
	}

	myWeaponsTypes.insert(std::pair<std::string, WeaponDataType>(weaponDataType.myType, weaponDataType));

	weaponDocument.CloseDocument();
}

void WeaponFactory::ReloadWeapon(const std::string&)
{
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::RELOAD_LEVEL));
}