#pragma once
#include "Entity.h"
#include "BulletMessage.h"
#include "Subscriber.h"
#include "StaticArray.h"

class BulletComponent;
struct BulletData
{
	CU::GrowingArray<Entity*> myPlayerBullets;
	CU::GrowingArray<Entity*> myEnemyBullets;

	//storing the bulletcomponents so that we dont have
	//to find the component in entity-map every frame, 
	//which is expensive
	CU::GrowingArray<BulletComponent*> myPlayerBulletComponents;
	CU::GrowingArray<BulletComponent*> myEnemyBulletComponents;

	eBulletType myType;
	float mySpeed;
	int myPlayerBulletCounter;
	int myEnemyBulletCounter;
	int myMaxBullet;
};


namespace Prism
{
	class Camera;
	class Instance;
	class Scene;
}

namespace tinyxml2
{
	class XMLElement;
}

class CollisionManager;
class EntityFactory;
class WeaponFactory;

class BulletManager : public Subscriber
{
public:
	BulletManager(CollisionManager& aCollisionManager, Prism::Scene& aScene);
	~BulletManager();


	void LoadFromFactory(WeaponFactory* aWeaponFactory, EntityFactory* aEntityFactory, 
			const std::string& aProjectileList);

	void Update(float aDeltaTime);
	void ReceiveMessage(const BulletMessage& aMessage) override;

	void RenderStreaks();

	CU::GrowingArray<Prism::Instance*>& GetInstances();

	BulletData* GetBullet(int anIndex);

private:
	void operator=(BulletManager&) = delete;

	void LoadProjectile(WeaponFactory* aWeaponFactory, EntityFactory* aEntityFactory, 
		const std::string& aProjectilePath);

	void ActivateBullet(BulletData* aWeaponData, const CU::Matrix44<float>& anOrientation, eEntityType aEntityType
		, const CU::Vector3<float>& aEnitityVelocity, const CU::Vector3<float>& aDirection, Entity* aHomingTarget
		, float aHomingTurnRateModifier);
	
	void UpdateBullet(BulletData* aWeaponData, const float& aDeltaTime);

	void DeleteWeaponData(BulletData* aWeaponData);

	CU::StaticArray<BulletData*, static_cast<int>(eBulletType::COUNT)> myBulletDatas;

	CollisionManager& myCollisionManager;
	Prism::Scene& myScene;

	// temp!!
	CU::GrowingArray<Prism::Instance*> myInstances;

};