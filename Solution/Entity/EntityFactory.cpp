#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityFactory.h"
#include "EntityEnumConverter.h"
#include <string>


EntityFactory* EntityFactory::myInstance = nullptr;

EntityFactory* EntityFactory::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new EntityFactory();
	}
	return myInstance;
}

void EntityFactory::Destroy()
{
	SAFE_DELETE(myInstance);
}

void EntityFactory::LoadEntities(const char* aEntityListXML)
{
	XMLReader entityListDocument;
	entityListDocument.OpenDocument(aEntityListXML);

	tinyxml2::XMLElement* rootElement = entityListDocument.ForceFindFirstChild("root");

	for (tinyxml2::XMLElement* e = entityListDocument.FindFirstChild(rootElement); e != nullptr; 
		e = entityListDocument.FindNextElement(e))
	{
		std::string entityPath;
		entityListDocument.ForceReadAttribute(e, "src", entityPath);
		LoadEntity(entityPath.c_str());
	}

	entityListDocument.CloseDocument();
}

Entity* EntityFactory::CreateEntity(eOwnerType aOwner, eEntityType aType, Prism::eOctreeType aOctreeType, 
	Prism::Scene& aScene, CU::Vector3f aPostion, const Prism::Terrain& aTerrian, const CU::Vector3f& aRotation, 
	const CU::Vector3f& aScale)
{
	if (myInstance->myLoadedEntityData.find(aType) != myInstance->myLoadedEntityData.end())
	{
		EntityData loadedEntityData = myInstance->myLoadedEntityData.find(aType)->second;
		Entity* newEntity = new Entity(aOwner, aOctreeType, loadedEntityData, aScene, aPostion, aTerrian, aRotation, aScale);
		return newEntity;
	}
	DL_ASSERT("Entity not found.");
	return nullptr;
}

Entity* EntityFactory::CreateEntity(eOwnerType aOwner, eEntityType aType, ePropType aPropType, Prism::eOctreeType aOctreeType,
	Prism::Scene& aScene, CU::Vector3f aPostion, const Prism::Terrain& aTerrian,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
{
	if (aType == eEntityType::PROP)
	{
		if (myInstance->myLoadedPropData.find(aPropType) != myInstance->myLoadedPropData.end())
		{
			EntityData loadedEntityData = myInstance->myLoadedPropData.find(aPropType)->second;
			Entity* newEntity = new Entity(aOwner, aOctreeType, loadedEntityData, aScene, aPostion, aTerrian, aRotation, aScale);
			newEntity->myPropType = aPropType;
			return newEntity;
		}
	}
	std::string errorMessage = "Prop " + std::to_string(static_cast<int>(aPropType)) + " not found.";
	DL_ASSERT(errorMessage);
	return nullptr;
}

Entity* EntityFactory::CreateEntity(eOwnerType aOwner, eEntityType aType, eUnitType aUnitType, Prism::eOctreeType aOctreeType,
	Prism::Scene& aScene, CU::Vector3f aPostion, const Prism::Terrain& aTerrian,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
{
	if (aType == eEntityType::UNIT)
	{
		if (myInstance->myLoadedUnitData.find(aUnitType) != myInstance->myLoadedUnitData.end())
		{
			EntityData loadedEntityData = myInstance->myLoadedUnitData.find(aUnitType)->second;
			Entity* newEntity = new Entity(aOwner, aOctreeType, loadedEntityData, aScene, aPostion, aTerrian, aRotation, aScale);
			newEntity->myUnitType = aUnitType;
			return newEntity;
		}
	}
	DL_ASSERT("Prop not found.");
	return nullptr;
}

void EntityFactory::LoadEntity(const char* aEntityPath)
{
	XMLReader entityDocument;
	entityDocument.OpenDocument(aEntityPath);

	tinyxml2::XMLElement* rootElement = entityDocument.ForceFindFirstChild("root");
	tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity");

	EntityData newData;
	std::string entityType;
	std::string entitySubType;
	entityDocument.ForceReadAttribute(entityElement, "type", entityType);
	newData.myType = EntityEnumConverter::ConvertStringToEntityType(CU::ToLower(entityType));
	if (newData.myType == eEntityType::PROP)
	{
		entityDocument.ForceReadAttribute(entityElement, "subType", entitySubType);
		newData.myPropType = EntityEnumConverter::ConvertStringToPropType(CU::ToLower(entitySubType));
	}
	else if (newData.myType == eEntityType::UNIT)
	{
		entityDocument.ForceReadAttribute(entityElement, "subType", entitySubType);
		newData.myUnitType = EntityEnumConverter::ConvertStringToUnitType(CU::ToLower(entitySubType));
	}


	for (tinyxml2::XMLElement* e = entityDocument.FindFirstChild(entityElement); e != nullptr; 
		e = entityDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("AnimationComponent"))
		{
			if (newData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You have a GraphicsComponent so you can't have a AnimationComponent");
			if (newData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You already have a AnimationComponent");

			myComponentLoader->LoadAnimationComponent(entityDocument, e, newData.myAnimationData);
		}
		else if (elementName == CU::ToLower("GraphicsComponent"))
		{
			if (newData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You have a AnimationComponent so you can't have a GraphicsComponent");
			if (newData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You already have a GraphicsComponent");

			myComponentLoader->LoadGraphicsComponent(entityDocument, e, newData.myGraphicsData);
		}
		else if (elementName == CU::ToLower("BuildingComponent"))
		{
			if (newData.myBuildingData.myExistsInEntity == true) DL_ASSERT("You already have a BuildingComponent");

			myComponentLoader->LoadBuidlingComponent(entityDocument, e, newData.myBuildingData);
		}
		else if (elementName == CU::ToLower("TriggerComponent"))
		{
			if (newData.myTriggerData.myExistsInEntity == true) DL_ASSERT("You already have a TriggerComponent");

			myComponentLoader->LoadTriggerComponent(entityDocument, e, newData.myTriggerData);
		}
		else if (elementName == CU::ToLower("CollisionComponent"))
		{
			if (newData.myCollisionData.myExistsInEntity == true) DL_ASSERT("You already have a CollisionComponent");

			myComponentLoader->LoadCollisionComponent(entityDocument, e, newData.myCollisionData);
		}
		else if (elementName == CU::ToLower("ActorComponent"))
		{
			if (newData.myActorData.myExistsInEntity == true) DL_ASSERT("You already have a ActorComponent");

			myComponentLoader->LoadActorComponent(entityDocument, e, newData.myActorData);
		}
		else if (elementName == CU::ToLower("ControllerComponent"))
		{
			if (newData.myControllerData.myExistsInEntity == true) DL_ASSERT("You already have a ControllerComponent");

			myComponentLoader->LoadControllerComponent(entityDocument, e, newData.myControllerData);
		}
		else if (elementName == CU::ToLower("HealthComponent"))
		{
			if (newData.myHealthData.myExistsInEntity == true) DL_ASSERT("You already have a HealthComponent");

			myComponentLoader->LoadHealthComponent(entityDocument, e, newData.myHealthData);
		}
		else 
		{
			std::string errorMessage = "The component " + elementName + 
				" is not Supported. Please check if you wrote right else tell a programmer.";
			DL_ASSERT(errorMessage.c_str());
		}
	}
	if (newData.myType == eEntityType::PROP)
	{
		myLoadedPropData[newData.myPropType] = newData;
	}
	else if (newData.myType == eEntityType::UNIT)
	{
		myLoadedUnitData[newData.myUnitType] = newData;
	}
	else 
	{
		myLoadedEntityData[newData.myType] = newData;
	}

	entityDocument.CloseDocument();
}

EntityFactory::EntityFactory()
{
	myComponentLoader = new ComponentLoader();
}


EntityFactory::~EntityFactory()
{
	SAFE_DELETE(myComponentLoader);
}