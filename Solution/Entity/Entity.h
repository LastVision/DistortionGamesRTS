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
	enum class eOctreeType;
}

class Component;


class Entity
{
	friend class AnimationComponent;
	friend class GraphicsComponent;
public:
	Entity(eEntityType aType, Prism::Scene& aScene, Prism::eOctreeType anOctreeType, const std::string& aName = ""
		, const CU::Vector3<float> aStartPosition = { 0.f, 0.f, 0.f });
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
	eEntityType GetType() const;
	eEntityState GetState() const;
	void SetState(eEntityState aState){ myState = aState; };
	Prism::eOctreeType GetOctreeType() const;
	bool GetAlive() const;
	void Kill();
	const std::string& GetName() const;
	void SetName(const std::string& aName);
	void Reset();


private:
	void operator=(Entity&) = delete;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;
	
	bool myAlive;
	std::string myName;
	const eEntityType myType;
	eEntityState myState;
	const Prism::eOctreeType myOctreeType;
	Prism::Scene& myScene;

	CU::Matrix44<float> myOrientation;
};

//
//template <typename T>
//T* Entity::AddComponent()
//{
//	DL_ASSERT_EXP(T::GetType() != eComponentType::NOT_USED, "Tried to add invalid component.");
//
//	int index = static_cast<int>(T::GetType());
//	if (myComponents[index] != nullptr)
//	{
//		DL_ASSERT("Tried to add a component twice to the same entity.");
//	}
//
//	T* component = new T(*this);
//	myComponents[index] = component;
//
//	return component;
//}

//template <typename T>
//void Entity::RemoveComponent()
//{
//	DL_ASSERT_EXP(T::GetType() != eComponentType::NOT_USED, "Tried to add invalid component.");
//
//	int index = static_cast<int>(T::GetTypeStatic());
//	if (myComponents[index] == nullptr)
//	{
//		DL_ASSERT("Tried to remove a component from a entity that wasnt added.");
//	}
//
//	delete myComponents[index];
//	myComponents[index] = nullptr;
//}

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

inline const std::string& Entity::GetName() const
{
	return myName;
}

inline void Entity::SetName(const std::string& aName)
{
	myName = aName;
}