#include "stdafx.h"

#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "Component.h"
#include "ControllerComponent.h"
#include "CollisionComponent.h"
#include "GraphicsComponent.h"
#include "Entity.h"
#include "EntityData.h"
#include "MovementComponent.h"
#include <Scene.h>
#include <Terrain.h>

Entity::Entity(eOwnerType aOwner, Prism::eOctreeType anOctreeType, EntityData& aEntityData
		, Prism::Scene& aScene, const CU::Vector3<float> aStartPosition, const Prism::Terrain& aTerrain)
	: myAlive(true)
	, myOwner(aOwner)
	, myScene(aScene)
	, myOctreeType(anOctreeType)
	, myState(eEntityState::IDLE)
	, myType(aEntityData.myType)
	, mySelected(false)
	, myHovered(false)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}

	myOrientation.SetPos(aStartPosition);
	aTerrain.CalcEntityHeight(myOrientation);

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

	if (aEntityData.myCollisionData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::COLLISION)] = new CollisionComponent(*this, aEntityData.myCollisionData);
	}

	if (aEntityData.myActorData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::ACTOR)] = new ActorComponent(*this, aEntityData.myActorData);
	}

	if (aEntityData.myControllerData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::CONTROLLER)] = new ControllerComponent(*this, aEntityData.myControllerData);
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

	if (mySelected == true)
	{
		Prism::RenderBox(myOrientation.GetPos());
	}
	else if (myHovered == true)
	{
		Prism::RenderBox(myOrientation.GetPos(), eColorDebug::WHITE);
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

void Entity::SetSelect(bool aStatus)
{
	mySelected = aStatus;
}

bool Entity::IsSelected() const
{
	return mySelected;
}

void Entity::SetHovered(bool aStatus)
{
	myHovered = aStatus;
}

bool Entity::IsHovered() const
{
	return myHovered;
}