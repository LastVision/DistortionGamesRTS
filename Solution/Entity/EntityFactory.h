#pragma once
#include <EngineEnums.h>
#include "EntityData.h"
#include <unordered_map>
#include <Vector.h>
#include "Entity.h"

class ComponentLoader;

namespace Prism
{
	class Scene;
	class Terrain;
}

class EntityFactory
{
public:
	static EntityFactory* GetInstance();
	static void Destroy();

	void LoadEntities(const char* aEntityListXML);
	static Entity* CreateEntity(eOwnerType aOwner, eEntityType aType, Prism::eOctreeType aOctreeType, 
		Prism::Scene& aScene, CU::Vector3f aPostion, const Prism::Terrain& aTerrian, 
		const CU::Vector3f& aRotation = CU::Vector3f(), const CU::Vector3f& aScale = { 1.f, 1.f, 1.f });

	static const eEntityType ConvertStringToEntityType(const std::string& entityType);
private:
	void LoadEntity(const char* aEntityPath);
	EntityFactory();
	~EntityFactory();
	std::unordered_map<eEntityType, EntityData> myLoadedEntityData;

	ComponentLoader* myComponentLoader;

	static EntityFactory* myInstance;
};

