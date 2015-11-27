#include "stdafx.h"

#include "AnimationComponent.h"
#include "Component.h"
#include "GraphicsComponent.h"
#include "Entity.h"
#include "EntityData.h"
#include "MovementComponent.h"
#include <Scene.h>

Entity::Entity(eOwnerType aOwner, Prism::eOctreeType anOctreeType, EntityData& aEntityData,
	Prism::Scene& aScene, const CU::Vector3<float> aStartPosition, Prism::Terrain& aTerrain)
	: myAlive(true)
	, myOwner(aOwner)
	, myScene(aScene)
	, myOctreeType(anOctreeType)
	, myState(eEntityState::IDLE)
	, myType(aEntityData.myType)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}

	myOrientation.SetPos(aStartPosition);

	if (aEntityData.myAnimationData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::ANIMATION)] = new AnimationComponent(*this, aEntityData.myAnimationData);
		myScene.AddInstance(GetComponent<AnimationComponent>()->GetInstance());
	}
	else if (aEntityData.myGraphicsData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::GRAPHICS)] = new GraphicsComponent(*this, aEntityData.myGraphicsData);
		myScene.AddInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	if (aEntityData.myMovementData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::MOVEMENT)] = new MovementComponent(*this, aEntityData.myMovementData, aTerrain);
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

void Entity::AddComponent(Component* aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent->GetType())] == nullptr, "Tried to add component several times");
	myComponents[int(aComponent->GetType())] = aComponent;
}

void Entity::RemoveComponent(eComponentType aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent)] != nullptr, "Tried to remove an nonexisting component");
	delete myComponents[int(aComponent)];
	myComponents[int(aComponent)] = nullptr;
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