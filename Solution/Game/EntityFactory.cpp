#include "stdafx.h"

#include "AIComponent.h"
#include "CollisionComponent.h"
#include "BulletComponent.h"
#include <CommonHelper.h>
#include <EngineEnums.h>
#include "Entity.h"
#include "EntityFactory.h"
#include "ParticleEmitterComponent.h"
#include <FileWatcher.h>
#include "GameStateMessage.h"
#include "GraphicsComponent.h"
#include "HealthComponent.h"
#include <Instance.h>
#include <MathHelper.h>
#include "PhysicsComponent.h"
#include "PlanetCollisionComponent.h"
#include "PostMaster.h"
#include "PowerUpComponent.h"
#include <Scene.h>
#include "ShootingComponent.h"
#include "StreakEmitterComponent.h"
#include "SoundComponent.h"
#include "WeaponFactory.h"
#include <XMLReader.h>

EntityData::EntityData(Prism::Scene& aDummyScene)
	: myEntity(new Entity(eEntityType::NOT_USED, aDummyScene, Prism::eOctreeType::NOT_IN_OCTREE))
{
}

EntityFactory::EntityFactory(WeaponFactory* aWeaponFactory)
	: myWeaponFactoryPointer(aWeaponFactory)
{
	myDummyScene = new Prism::Scene();
}

EntityFactory::~EntityFactory()
{
	for (auto it = myEntities.begin(); it != myEntities.end(); ++it)
	{
		delete it->second.myEntity;
	}

	myEntities.clear();

	delete myDummyScene;
}

void EntityFactory::LoadEntites(const std::string& aEntityRootPath, float aDifficultScale)
{
	myDifficultScale = aDifficultScale;
	XMLReader rootDocument;
	rootDocument.OpenDocument(aEntityRootPath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");

	WATCH_FILE(aEntityRootPath, EntityFactory::ReloadEntity);

	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string entityPath = "";
		rootDocument.ForceReadAttribute(e, "src", entityPath);
		if (entityPath != "")
		{
			//#ifdef _DEBUG
			//			XMLReader entityReader;
			//			entityReader.OpenDocument(entityPath);
			//			tinyxml2::XMLElement* entityElement;
			//			tinyxml2::XMLElement* rootElement = entityReader.FindFirstChild("root");
			//			if (rootElement == nullptr)
			//			{
			//				entityElement = entityReader.FindFirstChild("Entity");
			//			}
			//			else
			//			{
			//				entityElement = entityReader.FindFirstChild(rootElement, "Entity");
			//			}
			//
			//			std::string entityName;
			//			entityReader.ForceReadAttribute(entityElement, "name", entityName);
			//			myEntityTags[entityName] = entityPath;
			//			entityReader.CloseDocument();
			//#else
			LoadEntity(entityPath, aDifficultScale);
			WATCH_FILE(entityPath, EntityFactory::ReloadEntity);
			//#endif
		}
	}

	rootDocument.CloseDocument();
}

void EntityFactory::LoadEntity(const std::string& aEntityPath, float aDifficultScale)
{
	XMLReader entityDocument;
	entityDocument.OpenDocument(aEntityPath);

	EntityData newEntity(*myDummyScene);
	tinyxml2::XMLElement* entityElement;
	tinyxml2::XMLElement* rootElement = entityDocument.FindFirstChild("root");
	if (rootElement == nullptr)
	{
		entityElement = entityDocument.FindFirstChild("Entity");
	}
	else
	{
		entityElement = entityDocument.FindFirstChild(rootElement, "Entity");
	}

	std::string entityName = "";
	entityDocument.ForceReadAttribute(entityElement, "name", entityName);

	if (myEntities.find(entityName) != myEntities.end())
	{
		std::string errorMessage = "[EntityFactory] Entity there is already a object named " + entityName;
		DL_ASSERT(errorMessage.c_str());
	}
	newEntity.myEntity->SetName(entityName);
	if (entityName != "E_projectile_enemy_fast")
	{
		aDifficultScale = 1.f;
	}
	else {
		aDifficultScale = aDifficultScale;
	}
	ENTITY_LOG("Load entity %s starting", entityName.c_str());
	for (tinyxml2::XMLElement* e = entityDocument.FindFirstChild(entityElement); e != nullptr;
		e = entityDocument.FindNextElement(e))
	{
		std::string childName = e->Name();
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("GraphicsComponent").c_str()) == 0)
		{
			LoadGraphicsComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("AIComponent").c_str()) == 0)
		{
			LoadAIComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("ShootingComponent").c_str()) == 0)
		{
			LoadShootingComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("CollisionComponent").c_str()) == 0)
		{
			LoadCollisionComponent(newEntity, entityDocument, e, eCollisionType::NORMAL);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("PlanetCollisionComponent").c_str()) == 0)
		{
			LoadCollisionComponent(newEntity, entityDocument, e, eCollisionType::PLANET);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("PhysicsComponent").c_str()) == 0)
		{
			LoadPhysicsComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("BulletComponent").c_str()) == 0)
		{
			LoadBulletComponent(newEntity, entityDocument, e, aDifficultScale);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("HealthComponent").c_str()) == 0)
		{
			LoadHealthComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("PowerUpComponent").c_str()) == 0)
		{
			LoadPowerUpComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("ParticleEmitterComponent").c_str()) == 0)
		{
			LoadParticleEmitterComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("StreakEmitterComponent").c_str()) == 0)
		{
			LoadStreakEmitterComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("rotate").c_str()) == 0)
		{
			bool rotate;
			entityDocument.ReadAttribute(e, "value", rotate);
			newEntity.myEntity->SetShouldRotate(rotate);

			entityDocument.ReadAttribute(e, "xAxis", rotate);
			newEntity.myEntity->SetShouldRotateX(rotate);

			entityDocument.ReadAttribute(e, "yAxis", rotate);
			newEntity.myEntity->SetShouldRotateY(rotate);

			entityDocument.ReadAttribute(e, "zAxis", rotate);
			newEntity.myEntity->SetShouldRotateZ(rotate);

		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("SoundComponent").c_str()) == 0)
		{
			LoadSoundComponent(newEntity, entityDocument, e);
			ENTITY_LOG("Entity %s loaded %s", entityName.c_str(), e->Name());
		}

		else
		{
			std::string errorMessage = "[EntityFactory]: Entity could not find the component "
				+ static_cast<std::string>(e->Name());
			DL_ASSERT(errorMessage.c_str());
		}
	}
	if (entityName != "")
	{
		myEntities.insert(std::pair<std::string, EntityData>(entityName, newEntity));
		ENTITY_LOG("Load entity %s ending", entityName.c_str());
	}
	else
	{
		DL_ASSERT("[EntityFactory]: Entity could not be created missing a name.");
	}

	entityDocument.CloseDocument();
}

void EntityFactory::LoadAIComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aAIComponentElement)
{
	aEntityToAddTo.myEntity->AddComponent<AIComponent>();
	for (tinyxml2::XMLElement* e = aAIComponentElement->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("FollowEntity").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "targetName", aEntityToAddTo.myTargetName);
		}
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Speed").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "min", aEntityToAddTo.myMinSpeed);
			aDocument.ForceReadAttribute(e, "max", aEntityToAddTo.myMaxSpeed);
		}
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("TimeToNextDecision").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "min", aEntityToAddTo.myMinTimeToNextDecision);
			aDocument.ForceReadAttribute(e, "max", aEntityToAddTo.myMaxTimeToNextDecision);
		}
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("AvoidanceDistance").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "value", aEntityToAddTo.myAIAvoidanceDistance);
		}
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("AvoidanceOffset").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "x", aEntityToAddTo.myAIAvoidancePoint.x);
			aDocument.ForceReadAttribute(e, "y", aEntityToAddTo.myAIAvoidancePoint.y);
			aDocument.ForceReadAttribute(e, "z", aEntityToAddTo.myAIAvoidancePoint.z);
		}
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("AIMode").c_str()) == 0)
		{
			int aiMode = -1;
			aDocument.ForceReadAttribute(e, "value", aiMode);

			switch (aiMode)
			{
			case 1:
				aEntityToAddTo.myAITargetPositionMode = eAITargetPositionMode::KEEP_DISTANCE;
				break;
			case 2:
				aEntityToAddTo.myAITargetPositionMode = eAITargetPositionMode::ESCAPE_THEN_RETURN;
				break;
			case 3:
				aEntityToAddTo.myAITargetPositionMode = eAITargetPositionMode::KAMIKAZE;
				break;
			case 5:
				aEntityToAddTo.myAITargetPositionMode = eAITargetPositionMode::TURRET;
				break;
			default:
				DL_ASSERT("Invalid AI-mode");
				break;
			}
		}

		aEntityToAddTo.myAITurnRate = 1.f;
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("AITurnRate").c_str()) == 0)
		{
			aDocument.ReadAttribute(e, "value", aEntityToAddTo.myAITurnRate);
		}
	}
}

void EntityFactory::LoadBulletComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aBulletComponentElement, float aDifficultScale)
{
	aEntityToAddTo.myDamageRadius = 0.f;
	aEntityToAddTo.myEntity->AddComponent<BulletComponent>();
	for (tinyxml2::XMLElement* e = aBulletComponentElement->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("lifeTime").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "value", aEntityToAddTo.myMaxTime);
		}
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("damage").c_str()) == 0)
		{
			int damage = 0;

			aDocument.ForceReadAttribute(e, "value", damage);

			aEntityToAddTo.myDamage = int(damage * aDifficultScale);
		}
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("damageRadius").c_str()) == 0)
		{
			float radius = 0;
			aDocument.ForceReadAttribute(e, "value", radius);
			aEntityToAddTo.myDamageRadius = radius;
		}
	}
}

void EntityFactory::LoadCollisionComponent(EntityData& aEntityToAddTo, XMLReader& aDocument
	, tinyxml2::XMLElement* aCollisionComponenetElement, eCollisionType aCollisionType)
{
	if (aCollisionType == eCollisionType::NORMAL)
	{
		aEntityToAddTo.myEntity->AddComponent<CollisionComponent>();
	}
	else if (aCollisionType == eCollisionType::PLANET)
	{
		aEntityToAddTo.myEntity->AddComponent<PlanetCollisionComponent>();
	}

	for (tinyxml2::XMLElement* e = aCollisionComponenetElement->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("CollisionSphere").c_str()) == 0)
		{
			float radius = 0;

			aDocument.ForceReadAttribute(e, "radius", radius);

			aEntityToAddTo.myCollisionSphereRadius = radius;
		}
	}
}

void EntityFactory::LoadGraphicsComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aGraphicsComponentElement)
{
	aEntityToAddTo.myEntity->AddComponent<GraphicsComponent>();

	for (tinyxml2::XMLElement* e = aGraphicsComponentElement->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Model").c_str()) == 0)
		{
			std::string modelFile;
			std::string effectFile;

			aDocument.ForceReadAttribute(e, "modelFile", modelFile);
			aDocument.ForceReadAttribute(e, "effectFile", effectFile);

			aEntityToAddTo.myModelFile = modelFile;
			aEntityToAddTo.myEffectFile = effectFile;

			aEntityToAddTo.myGraphicsType = eEntityDataGraphicsType::MODEL;

			aEntityToAddTo.myEntity->GetComponent<GraphicsComponent>()->Init(modelFile.c_str(), effectFile.c_str());
		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Cube").c_str()) == 0)
		{
			float width = e->FloatAttribute("width");
			float height = e->FloatAttribute("height");
			float depth = e->FloatAttribute("depth");

			aDocument.ForceReadAttribute(e, "width", width);
			aDocument.ForceReadAttribute(e, "height", height);
			aDocument.ForceReadAttribute(e, "depth", depth);

			aEntityToAddTo.myWidth = width;
			aEntityToAddTo.myHeight = height;
			aEntityToAddTo.myDepth = depth;

			aEntityToAddTo.myGraphicsType = eEntityDataGraphicsType::CUBE;
			aEntityToAddTo.myEntity->GetComponent<GraphicsComponent>()->InitCube(width, height, depth);
		}

		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Scale").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "x", aEntityToAddTo.myScale.myX);
			aDocument.ForceReadAttribute(e, "y", aEntityToAddTo.myScale.myY);
			aDocument.ForceReadAttribute(e, "z", aEntityToAddTo.myScale.myZ);
		}
	}
}

void EntityFactory::LoadShootingComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aShootingComponenetElement)
{
	aEntityToAddTo.myEntity->AddComponent<ShootingComponent>();
	aDocument;
	for (tinyxml2::XMLElement* e = aShootingComponenetElement->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Weapon").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "type", aEntityToAddTo.myWeaponType);
		}
	}
}

void EntityFactory::LoadHealthComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aHealthComponentElement)
{
	aEntityToAddTo.myEntity->AddComponent<HealthComponent>();

	for (tinyxml2::XMLElement* e = aHealthComponentElement->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Health").c_str()) == 0)
		{
			int life = 0;

			aDocument.ForceReadAttribute(e, "value", life);

			aEntityToAddTo.myLife = life;
		}
	}
}

void EntityFactory::LoadPhysicsComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aPhysicsComponentElement)
{
	aEntityToAddTo.myEntity->AddComponent<PhysicsComponent>();

	for (tinyxml2::XMLElement* e = aPhysicsComponentElement->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Weight").c_str()) == 0)
		{
			float weight = 0;
			aDocument.ForceReadAttribute(e, "value", weight);
			aEntityToAddTo.myEntity->GetComponent<PhysicsComponent>()->Init(static_cast<int>(weight));
		}
	}
}

void EntityFactory::LoadPowerUpComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aPowerUpComponent)
{
	aEntityToAddTo.myDuration = 0.f;
	aEntityToAddTo.myPowerUpValue = 0.f;
	aEntityToAddTo.myUpgradePickupMessageTime = 0.f;
	aEntityToAddTo.myUpgradeName = "";
	aEntityToAddTo.myPowerUpName = "";
	aEntityToAddTo.myUpgradePickupMessage = "";
	aEntityToAddTo.myUpgradeID = -1;
	aEntityToAddTo.myEntity->AddComponent<PowerUpComponent>();

	for (tinyxml2::XMLElement* e = aPowerUpComponent->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Power").c_str()) == 0)
		{
			std::string name;
			aDocument.ForceReadAttribute(e, "type", name);
			aDocument.ForceReadAttribute(e, "value", aEntityToAddTo.myPowerUpValue);
			aDocument.ForceReadAttribute(e, "time", aEntityToAddTo.myDuration);
			aEntityToAddTo.myPowerUpType = ConvertToPowerUpType(name);

		}
		else if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("WeaponUpgrade").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "entityName", aEntityToAddTo.myUpgradeName);
			aDocument.ForceReadAttribute(e, "weaponID", aEntityToAddTo.myUpgradeID);
			aDocument.ForceReadAttribute(e, "pickupMessage", aEntityToAddTo.myUpgradePickupMessage);
			aDocument.ForceReadAttribute(e, "pickupMessageTime", aEntityToAddTo.myUpgradePickupMessageTime);
			aEntityToAddTo.myPowerUpType = ePowerUpType::WEAPON_UPGRADE;
		}
		aEntityToAddTo.myPowerUpName = ConvertToPowerUpInGameName(aEntityToAddTo.myPowerUpType);
	}
}

void EntityFactory::LoadParticleEmitterComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aParticleEmitterComponent)
{
	for (tinyxml2::XMLElement* e = aParticleEmitterComponent->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Path").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "src", aEntityToAddTo.myParticleEmitterXMLPath);
		}
	}
}

void EntityFactory::LoadStreakEmitterComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aStreakEmitterComponent)
{
	for (tinyxml2::XMLElement* e = aStreakEmitterComponent->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (std::strcmp(CU::ToLower(e->Name()).c_str(), CU::ToLower("Path").c_str()) == 0)
		{
			aDocument.ForceReadAttribute(e, "src", aEntityToAddTo.myStreakEmitterXMLPath);
		}
	}
}

void EntityFactory::LoadSoundComponent(EntityData& aEntityToAddTo, XMLReader& aDocument, tinyxml2::XMLElement* aSoundComponent)
{
	aDocument;
	aSoundComponent;
	aEntityToAddTo.myEntity->AddComponent<SoundComponent>();
}

void EntityFactory::CopyEntity(Entity* aTargetEntity, const std::string& aEntityTag)
{
	if (myEntities.find(aEntityTag) == myEntities.end())
	{
		//#ifdef _DEBUG
		//		if (myEntityTags.find(aEntityTag) == myEntityTags.end())
		//		{
		//			std::string error = "[EntityFactory] No entity with name " + aEntityTag;
		//			DL_ASSERT(error);
		//		}
		//		
		//		LoadEntity(myEntityTags[aEntityTag], myDifficultScale);
		//#else
		std::string error = "[EntityFactory] No entity with name " + aEntityTag;
		DL_ASSERT(error);
		//#endif
	}
	auto it = myEntities.find(aEntityTag);
	Entity* sourceEntity = it->second.myEntity;

	aTargetEntity->SetName(sourceEntity->GetName());

	if (aEntityTag.rfind("Asteroid") != std::string::npos 
		|| aEntityTag.rfind("asteroid") != std::string::npos 
		|| aEntityTag.rfind("Junk") != std::string::npos
		|| aEntityTag.rfind("junk") != std::string::npos)
	{
		int value = CU::Math::RandomRange(1, 6);
		aTargetEntity->SetShouldRotate(true);
		if (value == 1)
			aTargetEntity->SetShouldRotateX(true);
		else if (value == 2)
			aTargetEntity->SetShouldRotateY(true);
		else if (value == 3)
			aTargetEntity->SetShouldRotateZ(true);
		else if (value == 4)
		{
			aTargetEntity->SetShouldRotateX(true);
			aTargetEntity->SetShouldRotateY(true);
		}
		else if (value == 5)
		{
			aTargetEntity->SetShouldRotateY(true);
			aTargetEntity->SetShouldRotateZ(true);
		}
		else if (value == 6)
		{
			aTargetEntity->SetShouldRotateZ(true);
			aTargetEntity->SetShouldRotateX(true);
		}
	}
	else
	{
		aTargetEntity->SetShouldRotate(sourceEntity->GetShouldRotate());
		aTargetEntity->SetShouldRotateX(sourceEntity->GetShouldRotateX());
		aTargetEntity->SetShouldRotateY(sourceEntity->GetShouldRotateY());
		aTargetEntity->SetShouldRotateZ(sourceEntity->GetShouldRotateZ());
	}







	if (sourceEntity->GetComponent<CollisionComponent>() != nullptr)
	{
		eCollisionType collisionType = sourceEntity->GetComponent<CollisionComponent>()->GetCollisionType();

		if (collisionType == eCollisionType::NORMAL)
		{
			aTargetEntity->AddComponent<CollisionComponent>();
		}
		else if (collisionType == eCollisionType::PLANET)
		{
			aTargetEntity->AddComponent<PlanetCollisionComponent>();
		}

		aTargetEntity->GetComponent<CollisionComponent>()->Init(it->second.myCollisionSphereRadius);
	}
	if (sourceEntity->GetComponent<GraphicsComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<GraphicsComponent>();
		switch (it->second.myGraphicsType)
		{
		case eEntityDataGraphicsType::MODEL:
			aTargetEntity->GetComponent<GraphicsComponent>()->Init(it->second.myModelFile.c_str(),
				it->second.myEffectFile.c_str());
			break;
		case eEntityDataGraphicsType::CUBE:
			aTargetEntity->GetComponent<GraphicsComponent>()->InitCube(it->second.myWidth,
				it->second.myHeight, it->second.myDepth);
			break;
		default:
			break;
		}
		if (it->second.myScale != CU::Vector3f())
		{
			aTargetEntity->GetComponent<GraphicsComponent>()->SetScale(it->second.myScale);
			if (aTargetEntity->GetComponent<CollisionComponent>() != nullptr)
			{
				float scale = aTargetEntity->GetComponent<CollisionComponent>()->GetSphere().myRadius + it->second.myCollisionSphereRadius;
				scale *= CU::Math::GetMaximumValueFromVector(it->second.myScale);
				aTargetEntity->GetComponent<CollisionComponent>()->SetCollisionRadius(scale);
			}
		}
		else if (aTargetEntity->GetComponent<CollisionComponent>() != nullptr)
		{
			float scaleOrg = aTargetEntity->GetComponent<CollisionComponent>()->GetSphere().myRadius;
			aTargetEntity->GetComponent<CollisionComponent>()->SetCollisionRadius(scaleOrg + it->second.myCollisionSphereRadius);
		}
	}
	if (sourceEntity->GetComponent<PhysicsComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<PhysicsComponent>();
	}
	if (sourceEntity->GetComponent<AIComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<AIComponent>();
		float speed = CU::Math::RandomRange<float>(it->second.myMinSpeed, it->second.myMaxSpeed);
		float timeToNextDecision = CU::Math::RandomRange<float>(it->second.myMinTimeToNextDecision,
			it->second.myMaxTimeToNextDecision);
		aTargetEntity->GetComponent<AIComponent>()->Init(speed, timeToNextDecision, it->second.myTargetName
			, it->second.myAIAvoidanceDistance, it->second.myAIAvoidancePoint
			, it->second.myAITargetPositionMode, it->second.myAITurnRate);
	}
	if (sourceEntity->GetComponent<ShootingComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<ShootingComponent>();
		if (it->second.myWeaponType != "")
		{
			aTargetEntity->GetComponent<ShootingComponent>()->AddWeapon(myWeaponFactoryPointer->GetWeapon(it->second.myWeaponType));
		}
	}
	if (sourceEntity->GetComponent<HealthComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<HealthComponent>();
		aTargetEntity->GetComponent<HealthComponent>()->Init(it->second.myLife);
	}
	if (sourceEntity->GetComponent<BulletComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<BulletComponent>();
		if (it->second.myMaxTime > 0 && it->second.myDamage > 0)
		{
			eBulletType bulletType = ConvertToBulletType(sourceEntity->GetName());
			aTargetEntity->GetComponent<BulletComponent>()->Init(it->second.myMaxTime, it->second.myDamage, it->second.myDamageRadius, bulletType);
		}
	}
	if (sourceEntity->GetComponent<SoundComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<SoundComponent>();
	}

	if (sourceEntity->GetComponent<PowerUpComponent>() != nullptr)
	{
		aTargetEntity->AddComponent<PowerUpComponent>();

		if (it->second.myPowerUpType == ePowerUpType::WEAPON_UPGRADE)
		{
			aTargetEntity->GetComponent<PowerUpComponent>()->Init(it->second.myPowerUpType, it->second.myPowerUpName, it->second.myUpgradeName
				, it->second.myUpgradePickupMessage, it->second.myUpgradeID, it->second.myUpgradePickupMessageTime);
		}
		else
		{
			aTargetEntity->GetComponent<PowerUpComponent>()->Init(it->second.myPowerUpType, it->second.myPowerUpName, it->second.myPowerUpValue
				, it->second.myDuration);
		}
	}

	if (it->second.myParticleEmitterXMLPath != "")
	{
		aTargetEntity->AddComponent<ParticleEmitterComponent>()->Init(it->second.myParticleEmitterXMLPath);
	}

	if (it->second.myStreakEmitterXMLPath != "")
	{
		aTargetEntity->AddComponent<StreakEmitterComponent>()->Init(it->second.myStreakEmitterXMLPath);
	}

	ENTITY_LOG("Entity %s copying succeded", aTargetEntity->GetName().c_str());
}

void EntityFactory::ReloadEntity(const std::string&)
{
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::RELOAD_LEVEL));
}

ePowerUpType EntityFactory::ConvertToPowerUpType(std::string aName)
{
	std::string name = CU::ToLower(aName);
	if (name == "healthkit")
	{
		return ePowerUpType::HEALTHKIT;
	}
	else if (name == "shield")
	{
		return ePowerUpType::SHIELDBOOST;
	}
	else if (name == "firerate")
	{
		return ePowerUpType::FIRERATEBOOST;
	}
	else if (name == "weaponupgrade")
	{
		return ePowerUpType::WEAPON_UPGRADE;
	}
	else if (name == "emp")
	{
		return ePowerUpType::EMP;
	}
	else if (name == "homing")
	{
		return ePowerUpType::HOMING;
	}
	else if (name == "invulnerability")
	{
		return ePowerUpType::INVULNERABLITY;
	}

	std::string errorMessage = "[EntityFactory] There is no powerup named " + aName;
	DL_ASSERT(errorMessage.c_str());

	return ePowerUpType::NO_POWERUP;
}

std::string EntityFactory::ConvertToPowerUpInGameName(ePowerUpType aPowerUpType)
{
	if (aPowerUpType == ePowerUpType::HEALTHKIT)
	{
		return "Health kit";
	}
	else if (aPowerUpType == ePowerUpType::SHIELDBOOST)
	{
		return "Shield boost";
	}
	else if (aPowerUpType == ePowerUpType::FIRERATEBOOST)
	{
		return "Firerate";
	}
	else if (aPowerUpType == ePowerUpType::WEAPON_UPGRADE)
	{
		return "Upgrade";
	}
	else if (aPowerUpType == ePowerUpType::EMP)
	{
		return "EMP";
	}
	else if (aPowerUpType == ePowerUpType::HOMING)
	{
		return "Homing";
	}
	else if (aPowerUpType == ePowerUpType::INVULNERABLITY)
	{
		return "Invulnerability";
	}

	DL_ASSERT("[EntityFactory] Wrong powerup enum in ConvertToPowerUpInGameName.");
	return "";
}

eBulletType EntityFactory::ConvertToBulletType(std::string aName)
{
	std::string type = CU::ToLower(aName);
	if (type == "machinegunbullet1")
	{
		return eBulletType::MACHINGUN_BULLET_LEVEL_1;
	}
	else if (type == "machinegunbullet2")
	{
		return eBulletType::MACHINGUN_BULLET_LEVEL_2;
	}
	else if (type == "machinegunbullet3")
	{
		return eBulletType::MACHINGUN_BULLET_LEVEL_3;
	}
	else if (type == "rocketmissile1")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_1;
	}
	else if (type == "rocketmissile2")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_2;
	}
	else if (type == "rocketmissile3")
	{
		return eBulletType::ROCKET_MISSILE_LEVEL_3;
	}
	else if (type == "shotgunbullet1")
	{
		return eBulletType::SHOTGUN_BULLET_LEVEL_1;
	}
	else if (type == "shotgunbullet2")
	{
		return eBulletType::SHOTGUN_BULLET_LEVEL_2;
	}
	else if (type == "shotgunbullet3")
	{
		return eBulletType::SHOTGUN_BULLET_LEVEL_3;
	}
	else if (type == "e_projectile_enemy_default")
	{
		return eBulletType::ENEMY_BULLET_DEFAULT;
	}
	else if (type == "e_projectile_enemy_fast")
	{
		return eBulletType::ENEMY_BULLET_FAST;
	}
	else if (type == "e_projectile_enemy_slow")
	{
		return eBulletType::ENEMY_BULLET_SLOW;
	}
	else if (type == "e_projectile_enemy_turret")
	{
		return eBulletType::ENEMY_BULLET_TURRET;
	}
	else if (type == "e_projectile_enemy_homing")
	{
		return eBulletType::ENEMY_BULLET_HOMING;
	}
	std::string errorText = "The bullet " + aName + " is not a supported bullet. Please contact a programmer to fix this!";
	DL_ASSERT(errorText.c_str());
	return eBulletType::COUNT;
}