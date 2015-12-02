#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "Entity.h"
#include "EntityFactory.h"
#include <string>
#include <XMLReader.h>

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
		Entity* newEntity = new Entity(aOwner, aOctreeType, loadedEntityData, aScene, aPostion, aTerrian);
		return newEntity;
	}
	DL_ASSERT("Entity not found.");
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
	entityDocument.ForceReadAttribute(entityElement, "type", entityType);
	newData.myType = ConvertStringToEntityType(CU::ToLower(entityType));

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
		else 
		{
			std::string errorMessage = "The component " + elementName + 
				" is not Supported. Please check if you wrote right else tell a programmer.";
			DL_ASSERT(errorMessage.c_str());
		}
	}
	myLoadedEntityData[newData.myType] = newData;

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

const eEntityType EntityFactory::ConvertStringToEntityType(const std::string& entityType)
{
	if (entityType == "dragon")
	{
		return eEntityType::DRAGON;
	}
	else if(entityType == "dragonstatic")
	{
		return eEntityType::DRAGON_STATIC;
	}
	else if (entityType == "pinetree")
	{
		return eEntityType::PINE_TREE;
	}
	DL_ASSERT("This type is not supported, please tell Daniel about it.");
	return eEntityType::_COUNT;
}
