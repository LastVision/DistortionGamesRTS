#include "stdafx.h"

#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "BuildingComponent.h"
#include "Component.h"
#include "ControllerComponent.h"
#include "CollisionComponent.h"
#include "GraphicsComponent.h"
#include "HealthComponent.h"
#include "Entity.h"
#include "EntityData.h"
#include <Scene.h>
#include <Terrain.h>

Entity::Entity(eOwnerType aOwner, Prism::eOctreeType anOctreeType, EntityData& aEntityData
	, Prism::Scene& aScene, const CU::Vector3<float> aStartPosition, const Prism::Terrain& aTerrain
	,const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
	: myAlive(false)
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
	}
	else if (aEntityData.myGraphicsData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::GRAPHICS)] = new GraphicsComponent(*this, aEntityData.myGraphicsData);
		GetComponent<GraphicsComponent>()->SetRotation(aRotation);
		GetComponent<GraphicsComponent>()->SetScale(aScale);
	}

	if (aEntityData.myBuildingData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::BUILDING)] = new BuildingComponent(*this, aEntityData.myBuildingData);
	}

	if (aEntityData.myCollisionData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::COLLISION)] = new CollisionComponent(*this, aEntityData.myCollisionData);
	}

	if (aEntityData.myActorData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::ACTOR)] = new ActorComponent(*this, aEntityData.myActorData, aTerrain);
	}

	if (aEntityData.myControllerData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::CONTROLLER)] = new ControllerComponent(*this, aEntityData.myControllerData, aTerrain);
	}

	if (aEntityData.myActorData.myExistsInEntity == true)
	{
		DL_ASSERT_EXP(myComponents[static_cast<int>(eComponentType::CONTROLLER)] != nullptr
			, "ActorComponent wont work witout a ControllerComponent");
	}

	if (aEntityData.myHealthData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::HEALTH)] = new HealthComponent(*this, aEntityData.myHealthData);
	}
}

Entity::~Entity()
{
	AddToScene();
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

void Entity::AddToScene()
{
	if (GetComponent<GraphicsComponent>() != nullptr)
	{
		myScene.AddInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr)
	{
		myScene.AddInstance(GetComponent<AnimationComponent>()->GetInstance());
	}
}

void Entity::RemoveFromScene()
{
	if (GetComponent<GraphicsComponent>() != nullptr)
	{
		myScene.RemoveInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr)
	{
		myScene.RemoveInstance(GetComponent<AnimationComponent>()->GetInstance());
	}
}

void Entity::Kill()
{
	DL_ASSERT_EXP(myAlive == true, "Tried to kill an Entity multiple times");
	myAlive = false;
	RemoveFromScene();
}

void Entity::Spawn(const CU::Vector3f& aSpawnPosition)
{
	Reset();
	GetComponent<ControllerComponent>()->Spawn(aSpawnPosition);
	AddToScene();
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

CU::GrowingArray<CU::Vector2<float>> Entity::GetCutMesh() const
{
	CU::GrowingArray<CU::Vector2<float>> points(4);

	CU::Vector2<float> pos(myOrientation.GetPos().x, myOrientation.GetPos().z);

	float halfWidth = 0.5f;
	
	points.Add({ pos.x - halfWidth, pos.y - halfWidth });
	points.Add({ pos.x - halfWidth, pos.y + halfWidth });
	points.Add({ pos.x + halfWidth, pos.y + halfWidth });
	points.Add({ pos.x + halfWidth, pos.y - halfWidth });
	return points;
}