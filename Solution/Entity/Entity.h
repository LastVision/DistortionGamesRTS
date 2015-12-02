#pragma once

#include <DL_Debug.h>
#include "EntityEnum.h"
#include <unordered_map>
#include <StaticArray.h>
#include <Matrix.h>

namespace Prism
{
	class Camera;
	class Scene;
	class Terrain;
	enum class eOctreeType;
}

class Component;

struct EntityData;

class Entity
{
	friend class ActorComponent;
	friend class AnimationComponent;
	friend class ControllerComponent;
	friend class GraphicsComponent;

public:
	Entity(eOwnerType aOwner, Prism::eOctreeType anOctreeType, EntityData& aEntityData,
		Prism::Scene& aScene, const CU::Vector3<float> aStartPosition, const Prism::Terrain& aTerrain, 
		const CU::Vector3f& aRotation, const CU::Vector3f& aScale);
	~Entity();

	virtual void Update(float aDeltaTime);

	void AddComponent(Component* aComponent);
	void RemoveComponent(eComponentType aComponent);

	template <typename T>
	T* GetComponent();

	template <typename T>
	void SendNote(const T& aNote);

	const CU::Matrix44<float>& GetOrientation() const;
	Prism::Scene& GetScene();
	eOwnerType GetOwner() const;
	eEntityType GetType() const;
	eEntityState GetState() const;
	void SetState(eEntityState aState){ myState = aState; };
	Prism::eOctreeType GetOctreeType() const;
	bool GetAlive() const;
	void Kill();
	void Reset();

	void SetSelect(bool aStatus);
	bool IsSelected() const;

	void SetHovered(bool aStatus);
	bool IsHovered() const;

private:
	void operator=(Entity&) = delete;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;
	
	bool myAlive;
	const eEntityType myType;
	const eOwnerType myOwner;
	eEntityState myState;
	const Prism::eOctreeType myOctreeType;
	Prism::Scene& myScene;

	CU::Matrix44<float> myOrientation;

	bool mySelected;
	bool myHovered;
};

template <typename T>
T* Entity::GetComponent()
{
	return static_cast<T*>(myComponents[static_cast<int>(T::GetTypeStatic())]);
}

template <typename T>
void Entity::SendNote(const T& aMessage)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->ReceiveNote(aMessage);
		}
	}
}

inline const CU::Matrix44<float>& Entity::GetOrientation() const
{
	return myOrientation;
}

inline Prism::Scene& Entity::GetScene()
{
	return myScene;
}

inline eOwnerType Entity::GetOwner() const
{
	return myOwner;
}

inline eEntityType Entity::GetType() const
{
	return myType;
}

inline eEntityState Entity::GetState() const
{
	return myState;
}

inline Prism::eOctreeType Entity::GetOctreeType() const
{
	return myOctreeType;
}

inline bool Entity::GetAlive() const
{
	return myAlive;
}