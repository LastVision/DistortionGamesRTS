#include "stdafx.h"
#include "BulletAIComponent.h"
#include "BulletComponent.h"
#include "BulletManager.h"
#include <Camera.h>
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include <Engine.h>
#include <EngineEnums.h>
#include "Enums.h"
#include "Entity.h"
#include "EntityFactory.h"
#include <FileWatcher.h>
#include "GraphicsComponent.h"
#include <Instance.h>
#include "PostMaster.h"
#include "PhysicsComponent.h"
#include "StreakEmitterComponent.h"
#include "WeaponFactory.h"
#include <XMLReader.h>
#include "ShootingComponent.h"

BulletManager::BulletManager(CollisionManager& aCollisionManager, Prism::Scene& aScene)
	: myInstances(256)
	, myCollisionManager(aCollisionManager)
	, myScene(aScene)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ACTIVATE_BULLET, this);

	for (int i = 0; i < static_cast<int>(eBulletType::COUNT); ++i)
	{
		myBulletDatas[i] = nullptr;
	}
}

BulletManager::~BulletManager()
{
	for (int i = 0; i < static_cast<int>(eBulletType::COUNT); ++i)
	{
		DeleteWeaponData(myBulletDatas[i]);
	}

	PostMaster::GetInstance()->UnSubscribe(eMessageType::ACTIVATE_BULLET, this);
}

void BulletManager::Update(float aDeltaTime)
{
	for (int i = 0; i < static_cast<int>(eBulletType::COUNT); ++i)
	{
		if (myBulletDatas[i] != nullptr)
		{
			UpdateBullet(myBulletDatas[i], aDeltaTime);
		}
	}
}

void BulletManager::ReceiveMessage(const BulletMessage& aMessage)
{
	ActivateBullet(myBulletDatas[static_cast<int>(aMessage.GetBulletType())], aMessage.GetOrientation()
		, aMessage.GetEntityType(), aMessage.GetEntityVelocity(), aMessage.GetDirection(), aMessage.GetHomingTarget()
		, aMessage.GetHomingTurnRateModifier());
}

void BulletManager::LoadFromFactory(WeaponFactory* aWeaponFactory, EntityFactory* aEntityFactory, 
		const std::string& aProjectileList)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aProjectileList);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");

	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string projectilePath = "";
		rootDocument.ForceReadAttribute(e, "src", projectilePath);
		if (projectilePath != "")
		{
			LoadProjectile(aWeaponFactory, aEntityFactory, projectilePath);
		}
	}

	rootDocument.CloseDocument();
}

void BulletManager::LoadProjectile(WeaponFactory* aWeaponFactory, EntityFactory* aEntityFactory, 
		const std::string& aProjectilePath)
{
	std::string projectileLoaderType;
	XMLReader rootDocument;
	rootDocument.OpenDocument(aProjectilePath);
	tinyxml2::XMLElement* projectileElement;
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
	if (rootElement == nullptr) 
	{
		projectileElement = rootDocument.FindFirstChild("Projectile");
	}
	else
	{
		projectileElement = rootDocument.FindFirstChild(rootElement, "Projectile");
	}
	
	rootDocument.ForceReadAttribute(projectileElement, "type", projectileLoaderType);
	rootDocument.CloseDocument();

	BulletData* bulletData = new BulletData;
	ProjectileDataType projectileDataType = aWeaponFactory->GetProjectile(projectileLoaderType);


	bulletData->myMaxBullet = projectileDataType.myMaxBullet;
	bulletData->mySpeed = projectileDataType.mySpeed;

	bulletData->myPlayerBulletCounter = 0;
	bulletData->myPlayerBullets.Init(bulletData->myMaxBullet);
	bulletData->myPlayerBulletComponents.Init(bulletData->myMaxBullet);

	for (int i = 0; i < bulletData->myMaxBullet; i++)
	{
		Entity* newEntity = new Entity(eEntityType::PLAYER_BULLET, myScene, Prism::eOctreeType::NOT_IN_OCTREE);
		aEntityFactory->CopyEntity(newEntity, projectileDataType.myEntityType);
		newEntity->GetComponent<GraphicsComponent>()->SetPosition({ 0, 0, 0 });
		
		bulletData->myPlayerBulletComponents.Add(newEntity->GetComponent<BulletComponent>());
		bulletData->myPlayerBullets.Add(newEntity);
	}

	bulletData->myEnemyBulletCounter = 0;
	bulletData->myEnemyBullets.Init(bulletData->myMaxBullet);
	bulletData->myEnemyBulletComponents.Init(bulletData->myMaxBullet);
	for (int i = 0; i < bulletData->myMaxBullet; i++)
	{
		Entity* newEntity = new Entity(eEntityType::ENEMY_BULLET, myScene, Prism::eOctreeType::NOT_IN_OCTREE);
		aEntityFactory->CopyEntity(newEntity, projectileDataType.myEntityType);
		newEntity->GetComponent<GraphicsComponent>()->SetPosition({ 0, 0, 0 });
		if (projectileDataType.myCollision > 0)
		{
			newEntity->GetComponent<CollisionComponent>()->SetCollisionRadius(projectileDataType.myCollision);
		}
		bulletData->myEnemyBulletComponents.Add(newEntity->GetComponent<BulletComponent>());
		bulletData->myEnemyBullets.Add(newEntity);
	}

	bulletData->myType = ConvertToBulletEnum(projectileDataType.myType);

	if (bulletData->myType == eBulletType::COUNT)
	{
		std::string errorMessage = "[BulletManager] No bullet with name " + projectileDataType.myType;
		DL_ASSERT(errorMessage.c_str());
	}

	DeleteWeaponData(myBulletDatas[static_cast<int>(bulletData->myType)]);
	myBulletDatas[static_cast<int>(bulletData->myType)] = bulletData;
}

void BulletManager::ActivateBullet(BulletData* aWeaponData, const CU::Matrix44<float>& anOrientation
	, eEntityType aEntityType, const CU::Vector3<float>& aEnitityVelocity, const CU::Vector3<float>& aDirection
	, Entity* aHomingTarget, float aHomingTurnRateModifier)
{

	Entity* bullet = nullptr;
	if (aEntityType == eEntityType::PLAYER || aEntityType == eEntityType::ALLY)
	{
		bullet = aWeaponData->myPlayerBullets[aWeaponData->myPlayerBulletCounter];
	}
	else if (aEntityType == eEntityType::ENEMY)
	{
		bullet = aWeaponData->myEnemyBullets[aWeaponData->myEnemyBulletCounter];
	}
	DL_ASSERT_EXP(bullet != nullptr, "Non Player/Enemy cant activate bullets!");

	bullet->GetComponent<BulletComponent>()->SetOwner(aEntityType);

	if (bullet->GetComponent<BulletComponent>()->GetActive() == false)
	{
		if (aEntityType == eEntityType::PLAYER || aEntityType == eEntityType::ALLY)
		{
			myCollisionManager.Add(bullet->GetComponent<CollisionComponent>(), eEntityType::PLAYER_BULLET);
		}
		else if (aEntityType == eEntityType::ENEMY)
		{
			myCollisionManager.Add(bullet->GetComponent<CollisionComponent>(), eEntityType::ENEMY_BULLET);
		}
	}

	if (bullet->GetComponent<BulletAIComponent>() != nullptr)
	{
		bullet->RemoveComponent<BulletAIComponent>();
	}

	if (aEntityType == eEntityType::PLAYER)
	{
		bullet->GetComponent<PhysicsComponent>()->Init(anOrientation,
			(anOrientation.GetForward() * (aWeaponData->mySpeed)) + aEnitityVelocity);
	}
	else if (aEntityType == eEntityType::ENEMY || aEntityType == eEntityType::ALLY)
	{
		bullet->GetComponent<PhysicsComponent>()->Init(anOrientation,
			(aDirection * (aWeaponData->mySpeed)) + aEnitityVelocity);
	}
	
	bullet->GetComponent<BulletComponent>()->SetActive(true);
	bullet->GetComponent<CollisionComponent>()->Update(0.5f);

	if (bullet->GetComponent<StreakEmitterComponent>() != nullptr)
	{
		bullet->GetComponent<StreakEmitterComponent>()->Reset();
	}

	if (aHomingTarget != nullptr)
	{
		bullet->AddComponent<BulletAIComponent>()->Init((CU::Length((anOrientation.GetForward() * (aWeaponData->mySpeed)) + aEnitityVelocity) / 2.f)
			, aHomingTurnRateModifier, &myCollisionManager);
		bullet->GetComponent<BulletAIComponent>()->SetEntityToFollow(aHomingTarget, aHomingTarget);
	}

	if (aEntityType == eEntityType::PLAYER || aEntityType == eEntityType::ALLY)
	{
		aWeaponData->myPlayerBulletCounter++;
		if (aWeaponData->myPlayerBulletCounter >= aWeaponData->myMaxBullet)
		{
			aWeaponData->myPlayerBulletCounter = 0;
		}
	}
	else if (aEntityType == eEntityType::ENEMY)
	{
		aWeaponData->myEnemyBulletCounter++;
		if (aWeaponData->myEnemyBulletCounter >= aWeaponData->myMaxBullet)
		{
			aWeaponData->myEnemyBulletCounter = 0;
		}
	}
}

void BulletManager::UpdateBullet(BulletData* aWeaponData, const float& aDeltaTime)
{
	BulletComponent* playerBulletComp = nullptr;
	BulletComponent* enemyBulletComp = nullptr;

	for (int i = 0; i < aWeaponData->myMaxBullet; ++i)
	{
		playerBulletComp = aWeaponData->myPlayerBulletComponents[i];
		if (playerBulletComp->GetActive() == true)
		{
			aWeaponData->myPlayerBullets[i]->Update(aDeltaTime);

			if (playerBulletComp->GetActive() == false)
			{
				if (playerBulletComp->GetEntity().GetComponent<BulletAIComponent>() != nullptr)
				{
					playerBulletComp->GetEntity().RemoveComponent<BulletAIComponent>();
				}
				myCollisionManager.Remove(aWeaponData->myPlayerBullets[i]->GetComponent<CollisionComponent>()
					, eEntityType::PLAYER_BULLET);
			}
		}

		enemyBulletComp = aWeaponData->myEnemyBulletComponents[i];
		if (enemyBulletComp->GetActive() == true)
		{
			aWeaponData->myEnemyBullets[i]->Update(aDeltaTime);

			if (enemyBulletComp->GetActive() == false)
			{
				if (enemyBulletComp->GetEntity().GetComponent<BulletAIComponent>() != nullptr)
				{
					enemyBulletComp->GetEntity().RemoveComponent<BulletAIComponent>();
				}
				myCollisionManager.Remove(aWeaponData->myEnemyBullets[i]->GetComponent<CollisionComponent>()
					, eEntityType::ENEMY_BULLET);
			}
		}
	}
}

void BulletManager::DeleteWeaponData(BulletData* aWeaponData)
{
	if (aWeaponData != nullptr)
	{
		aWeaponData->myPlayerBullets.DeleteAll();
		aWeaponData->myEnemyBullets.DeleteAll();
	}

	delete aWeaponData;
	aWeaponData = nullptr;
}

void BulletManager::RenderStreaks()
{
	for (int i = 0; i < static_cast<int>(eBulletType::COUNT); ++i)
	{
		if (myBulletDatas[i] != nullptr)
		{
			for (int j = 0; j < myBulletDatas[i]->myMaxBullet; ++j)
			{
				if (myBulletDatas[i]->myPlayerBulletComponents[j]->GetActive() == true
					&& myBulletDatas[i]->myPlayerBullets[j]->GetComponent<StreakEmitterComponent>() != nullptr)
				{
					myBulletDatas[i]->myPlayerBullets[j]->GetComponent<StreakEmitterComponent>()->Render();
				}

				if (myBulletDatas[i]->myEnemyBulletComponents[j]->GetActive() == true
					&& myBulletDatas[i]->myEnemyBullets[j]->GetComponent<StreakEmitterComponent>() != nullptr)
				{
					myBulletDatas[i]->myEnemyBullets[j]->GetComponent<StreakEmitterComponent>()->Render();
				}
			}
		}
	}
}

CU::GrowingArray<Prism::Instance*>& BulletManager::GetInstances()
{
	myInstances.RemoveAll();

	for (int i = 0; i < static_cast<int>(eBulletType::COUNT); ++i)
	{
		if (myBulletDatas[i] != nullptr)
		{
			for (int j = 0; j < myBulletDatas[i]->myMaxBullet; ++j)
			{
				if (myBulletDatas[i]->myPlayerBulletComponents[j]->GetActive() == true)
				{
					myInstances.Add(myBulletDatas[i]->myPlayerBullets[j]->GetComponent<GraphicsComponent>()->GetInstance());
				}

				if (myBulletDatas[i]->myEnemyBulletComponents[j]->GetActive() == true)
				{
					myInstances.Add(myBulletDatas[i]->myEnemyBullets[j]->GetComponent<GraphicsComponent>()->GetInstance());
				}
			}
		}
	}
	return myInstances;
}

BulletData* BulletManager::GetBullet(int anIndex)
{
	return myBulletDatas[anIndex];
}
