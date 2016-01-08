#include "stdafx.h"

#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "BuildingComponent.h"
#include "CollisionComponent.h"
#include "ControllerComponent.h"
#include "EnrageComponent.h"
#include "GraphicsComponent.h"
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include "EntityId.h"
#include <Scene.h>
#include "SelectionComponent.h"
#include "SoundComponent.h"
#include <Terrain.h>
#include "TotemComponent.h"
#include "TriggerComponent.h"


Entity::Entity(eOwnerType aOwner, Prism::eOctreeType anOctreeType, EntityData& aEntityData
	, Prism::Scene& aScene, const CU::Vector3<float> aStartPosition, const Prism::Terrain& aTerrain
	, const CU::Vector3f& aRotation, const CU::Vector3f& aScale, eUnitType aUnitType)
	: myAlive(false)
	, myOwner(aOwner)
	, myScene(aScene)
	, myOctreeType(anOctreeType)
	, myState(eEntityState::IDLE)
	, myType(aEntityData.myType)
	, mySelected(false)
	, myHovered(false)
	, myPosition({aStartPosition.x, aStartPosition.z})
	, myPropType(ePropType::NOT_A_PROP)
	, myUnitType(aUnitType)
	, myDecayFlag(false)
{
	myId = EntityId::GetInstance()->GetId(this);
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}

	myOrientation.SetPos(aStartPosition);
	aTerrain.CalcEntityHeight(myOrientation);

	if (aEntityData.myAnimationData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::ANIMATION)] = new AnimationComponent(*this, aEntityData.myAnimationData, aTerrain);
	}
	else if (aEntityData.myGraphicsData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::GRAPHICS)] = new GraphicsComponent(*this, aEntityData.myGraphicsData);
		GetComponent<GraphicsComponent>()->SetRotation(aRotation);
		GetComponent<GraphicsComponent>()->SetScale(aScale);
	}
	//AddToScene();

	if (aEntityData.myBuildingData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::BUILDING)] = new BuildingComponent(*this, aEntityData.myBuildingData);
	}

	if (aEntityData.myCollisionData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::COLLISION)] = new CollisionComponent(*this, aEntityData.myCollisionData);
	}

	if (aEntityData.myTriggerData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::TRIGGER)] = new TriggerComponent(*this, aEntityData.myTriggerData);
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

	if (aEntityData.myTotemData.myExistsInEntity == true)
	{
		myOrientation.SetPos(aStartPosition);
		myComponents[static_cast<int>(eComponentType::TOTEM)] = new TotemComponent(*this, aEntityData.myTotemData);
	}

	if (aEntityData.myEnrageData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::ENRAGE)] = new EnrageComponent(*this, aEntityData.myEnrageData);
	}

	if (aEntityData.myGrenadeData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::GRENADE)] = new GrenadeComponent(*this, aEntityData.myGrenadeData);
	}

	if (myOwner == eOwnerType::PLAYER)
	{
		if (aEntityData.mySelectionData.myExistsInEntity == true)
		{
			myComponents[static_cast<int>(eComponentType::SELECTION)] = new SelectionComponent(*this, aEntityData.mySelectionData);
		}
	}


	if (aEntityData.mySoundData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::SOUND)] = new SoundComponent(*this);
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
	myPosition = { myOrientation.GetPos().x, myOrientation.GetPos().z };
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Update(aDeltaTime);
		}
	}

	if (myDecayFlag == true)
	{
		myCurrentDecayTime -= aDeltaTime;
		myOrientation.SetPos({ myPosition.x, myOrientation.GetPos().y - 0.05f * aDeltaTime, myPosition.y });
		if (myCurrentDecayTime < 0)
		{
			myCurrentDecayTime = 0;
		}
	}

	if (mySelected == true)
	{
		Prism::RenderBox(myOrientation.GetPos());
		//mySelectionCircle.Render(myOrientation.GetPos());
	}
	else if (myHovered == true)
	{
		Prism::RenderBox(myOrientation.GetPos(), eColorDebug::WHITE);
	}
	if (myComponents[static_cast<int>(eComponentType::ANIMATION)] != nullptr && myAlive == false && myState == eEntityState::DYING &&
		static_cast<AnimationComponent*>(myComponents[static_cast<int>(eComponentType::ANIMATION)])->IsCurrentAnimationDone() 
		&& myDecayFlag == false)
	{
		myCurrentDecayTime = 30.f;
		myDecayFlag = true;
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
	if (GetComponent<GraphicsComponent>() != nullptr && GetComponent<GraphicsComponent>()->GetInstance() != nullptr)
	{
		myScene.AddInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr && GetComponent<AnimationComponent>()->GetInstance() != nullptr)
	{
		myScene.AddInstance(GetComponent<AnimationComponent>()->GetInstance());
	}
	if (GetComponent<SelectionComponent>() != nullptr && GetComponent<SelectionComponent>()->GetInstance() != nullptr)
	{
		myScene.AddInstance(GetComponent<SelectionComponent>()->GetInstance());
	}

	myIsInScene = true;
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
	if (GetComponent<SelectionComponent>() != nullptr && GetComponent<SelectionComponent>()->GetIsRemovedFromScene() == false)
	{
		myScene.RemoveInstance(GetComponent<SelectionComponent>()->GetInstance());
	}

	myIsInScene = false;
}

void Entity::RemoveSelectionRingFromScene()
{
	if (GetComponent<SelectionComponent>() != nullptr)
	{
		myScene.RemoveInstance(GetComponent<SelectionComponent>()->GetInstance());
		GetComponent<SelectionComponent>()->SetIsRemovedFromScene(true);
	}
}

void Entity::Kill()
{
	DL_ASSERT_EXP(myAlive == true, "Tried to kill an Entity multiple times");
	myAlive = false;
	//RemoveFromScene();
}

void Entity::Spawn(const CU::Vector3f& aSpawnPosition)
{
	myState = eEntityState::IDLE;
	myDecayFlag = false;
	myOrientation.SetPos(aSpawnPosition);
	myPosition.x = aSpawnPosition.x;
	myPosition.y = aSpawnPosition.z;
	Reset();
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

bool Entity::GetShouldBeRemoved() const
{
	return myAlive == false && myState == eEntityState::DYING && myCurrentDecayTime == 0 && myDecayFlag == true &&
		static_cast<AnimationComponent*>(myComponents[static_cast<int>(eComponentType::ANIMATION)])->IsCurrentAnimationDone();
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

void Entity::SetPosition(const CU::Vector3f& aPosition)
{
	myOrientation.SetPos(aPosition);
	myPosition.x = aPosition.x;
	myPosition.y = aPosition.z;
}