#pragma once
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
public:
	Entity(eEntityType aType, Prism::Scene& aScene, Prism::eOctreeType anOctreeType, const std::string& aName = "");
	~Entity();

	virtual void Update(float aDeltaTime);

	template <typename T>
	T* AddComponent();

	template <typename T>
	void RemoveComponent();

	template <typename T>
	T* GetComponent();

	template <typename T>
	void SendNote(const T& aNote);

	CU::Matrix44<float> myOrientation;
	CU::Matrix44<float> myOriginalOrientation;

	eEntityType GetType() const;
	bool GetAlive() const;
	void Kill();
	const std::string& GetName() const;
	void SetName(const std::string& aName);
	void Reset();

	Prism::Scene& GetScene();

	void SetPlayerScene(Prism::Scene& aScene);

	Prism::eOctreeType GetOctreeType() const;

private:
	void operator=(Entity&) = delete;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;
	
	bool myAlive;
	std::string myName;
	const eEntityType myType;
	Prism::Scene* myScene;
	const Prism::eOctreeType myOctreeType;
};

template <typename T>
T* Entity::AddComponent()
{
	DL_ASSERT_EXP(T::GetType() != eComponentType::NOT_USED, "Tried to add invalid component.");

	int index = static_cast<int>(T::GetType());
	if (myComponents[index] != nullptr)
	{
		DL_ASSERT("Tried to add a component twice to the same entity.");
	}

	T* component = new T(*this);
	myComponents[index] = component;

	return component;
}

template <typename T>
void Entity::RemoveComponent()
{
	DL_ASSERT_EXP(T::GetType() != eComponentType::NOT_USED, "Tried to add invalid component.");

	int index = static_cast<int>(T::GetType());
	if (myComponents[index] == nullptr)
	{
		DL_ASSERT("Tried to remove a component from a entity that wasnt added.");
	}

	delete myComponents[index];
	myComponents[index] = nullptr;
}

template <typename T>
T* Entity::GetComponent()
{
	return static_cast<T*>(myComponents[static_cast<int>(T::GetType())]);
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

inline eEntityType Entity::GetType() const
{
	return myType;
}

inline bool Entity::GetAlive() const
{
	return myAlive;
}

inline Prism::Scene& Entity::GetScene()
{
	return *myScene;
}

inline void Entity::SetPlayerScene(Prism::Scene& aScene)
{
	myScene = &aScene;
}

inline const std::string& Entity::GetName() const
{
	return myName;
}

inline void Entity::SetName(const std::string& aName)
{
	myName = aName;
}