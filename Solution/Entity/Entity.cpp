#include "stdafx.h"

#include "Component.h"
#include "Entity.h"

Entity::Entity(eEntityType aType, Prism::Scene& aScene, Prism::eOctreeType anOctreeType, const std::string& aName)
	: myAlive(true)
	, myType(aType)
	, myScene(aScene)
	, myOctreeType(anOctreeType)
	, myName(aName)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}
}

Entity::~Entity()
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		delete myComponents[i];
		myComponents[i] = nullptr;
	}

}

void Entity::Update(float aDeltaTime)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Update(aDeltaTime);
		}
	}
}

void Entity::Kill()
{
	myAlive = false;
}

void Entity::Reset()
{
	myAlive = true;
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Reset();
		}
	}
}