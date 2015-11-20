#pragma once

class Entity;
class WeaponFactory;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

namespace Prism
{
	class Scene;
	enum class eOctreeType;
}

enum class eEntityDataGraphicsType
{
	MODEL,
	CUBE
};


struct EntityData
{
	EntityData(Prism::Scene& aDummyScene);

	Entity* myEntity;

	std::string myEffectFile;
	std::string myModelFile;
	std::string myTargetName;
	std::string myWeaponType;
	std::string myParticleEmitterXMLPath;
	std::string myStreakEmitterXMLPath;

	CU::Vector3f myScale;
	CU::Vector3f myAIAvoidancePoint;

	float myCollisionSphereRadius;
	float myDepth;
	float myHeight;
	float myMaxTime;
	float myMinSpeed;
	float myMaxSpeed;
	float myMinTimeToNextDecision;
	float myMaxTimeToNextDecision;
	float myAIAvoidanceDistance;
	eAITargetPositionMode myAITargetPositionMode = eAITargetPositionMode::NOT_USED;
	float myAITurnRate;
	float myWidth;

	int myChanceToFollow;

	float myDamageRadius;
	int myLife;
	int myDamage;

	float myPowerUpValue;
	float myDuration;
	ePowerUpType myPowerUpType;
	std::string myUpgradeName;
	std::string myPowerUpName;
	std::string myUpgradePickupMessage;
	float myUpgradePickupMessageTime;
	int myUpgradeID;

	eEntityDataGraphicsType myGraphicsType;
	Prism::eOctreeType myType;
};

class EntityFactory
{
public:
	EntityFactory(WeaponFactory* aWeaponFactory);
	~EntityFactory();

	void LoadEntites(const std::string& aEntityRootPath, float aDifficultScale = 1.f);

	void CopyEntity(Entity* aTargetEntity, const std::string& aEntityTag);
	void ReloadEntity(const std::string&);
private:
	void LoadEntity(const std::string& aEntityPath, float aDifficultScale = 1.f);

	void LoadAIComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aAIComponentElement);
	void LoadBulletComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aBulletComponentElement, float aDifficultScale);
	void LoadCollisionComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aCollisionComponenetElement, eCollisionType aCollisionType);
	void LoadGraphicsComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aGraphicsComponentElement);
	void LoadHealthComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aHealthComponentElement);
	void LoadShootingComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aShootingComponenetElement);
	void LoadPhysicsComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aPhysicsComponentElement);
	void LoadPowerUpComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aPowerUpComponent);
	void LoadParticleEmitterComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aParticleEmitterComponent);
	void LoadStreakEmitterComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aStreakEmitterComponent);
	void LoadSoundComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aSoundComponent);

	ePowerUpType ConvertToPowerUpType(std::string aName);
	std::string ConvertToPowerUpInGameName(ePowerUpType aPowerUpType);
	eBulletType ConvertToBulletType(std::string aName);

	std::unordered_map<std::string, EntityData> myEntities;
#ifdef _DEBUG
	std::unordered_map<std::string, std::string> myEntityTags;
#endif

	float myDifficultScale;

	Prism::Scene* myDummyScene;
	WeaponFactory* myWeaponFactoryPointer;
};

