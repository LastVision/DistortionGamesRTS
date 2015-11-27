#pragma once
#include <EngineEnums.h>
#include "EntityData.h"
#include <unordered_map>
#include <Vector.h>

class ComponentLoader;
class Entity;

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
		Prism::Scene& aScene, CU::Vector3f aPostion, Prism::Terrain& aTerrian);
private:
	void LoadEntity(const char* aEntityPath);
	EntityFactory();
	~EntityFactory();
	const eEntityType ConvertStringToEntityType(std::string entityType);
	std::unordered_map<eEntityType, EntityData> myLoadedEntityData;

	ComponentLoader* myComponentLoader;

	static EntityFactory* myInstance;
};

