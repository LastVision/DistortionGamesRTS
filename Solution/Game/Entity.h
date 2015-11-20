#pragma once
#include "Enums.h"
#include <unordered_map>
#include <StaticArray.h>

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

	void SetShouldRotate(bool aShouldRotate);
	void SetShouldRotateX(bool aShouldRotate);
	void SetShouldRotateY(bool aShouldRotate);
	void SetShouldRotateZ(bool aShouldRotate);

	bool GetShouldRotate();
	bool GetShouldRotateX();
	bool GetShouldRotateY();
	bool GetShouldRotateZ();

	void SetPowerUpName(const std::string& aPowerUpName);


	void SetPowerUp(ePowerUpType someType);
	ePowerUpType GetPowerUpType() const;
	Prism::eOctreeType GetOctreeType() const;

	void ActivateGUIStartRenderingMarker();
	float GetGUIStartRenderingMarkerTimer();
	bool GetGUIStartReneringMarker();
	//void ShakeCamera(float aRotationRate, float aMaxRotation, float aTime);

private:
	void operator=(Entity&) = delete;
	CU::StaticArray<Component*, static_cast<int>(eComponentType::_COUNT)> myComponents;
	
	bool myAlive;
	std::string myName;
	std::string myPowerUpName;
	const eEntityType myType;
	Prism::Scene* myScene;
	const Prism::eOctreeType myOctreeType;
	ePowerUpType myPowerUpType;

	bool myShouldRotate;
	bool myRotateX;
	bool myRotateY;
	bool myRotateZ;

	CU::Matrix44f myRotateXMatrix;
	CU::Matrix44f myRotateYMatrix;
	CU::Matrix44f myRotateZMatrix;

	bool myGUIMarkerStartRendering;
	float myGUIMarkerStartRenderingTime;

	//bool myShakeCamera;
	//float myRotateRate;
	//float myMaxShake;
	//float myCurrentShake;
	//float myMaxShakeTime;
	//float myCurrentShakeTime;

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

inline void Entity::SetPowerUp(ePowerUpType someType)
{
	myPowerUpType = someType;
}

inline ePowerUpType Entity::GetPowerUpType() const
{
	return myPowerUpType;
}

inline Prism::eOctreeType Entity::GetOctreeType() const
{
	return myOctreeType;
}

inline void Entity::SetShouldRotate(bool aShouldRotate)
{
	myShouldRotate = aShouldRotate;
}

inline bool Entity::GetShouldRotate()
{
	return myShouldRotate;
}

inline void Entity::SetShouldRotateX(bool aShouldRotate)
{
	myRotateX = aShouldRotate;
}

inline bool Entity::GetShouldRotateX()
{
	return myRotateX;
}

inline void Entity::SetShouldRotateY(bool aShouldRotate)
{
	myRotateY = aShouldRotate;
}

inline bool Entity::GetShouldRotateY()
{
	return myRotateY;
}

inline void Entity::SetShouldRotateZ(bool aShouldRotate)
{
	myRotateZ = aShouldRotate;
}

inline bool Entity::GetShouldRotateZ()
{
	return myRotateZ;
}

inline void Entity::ActivateGUIStartRenderingMarker()
{
	myGUIMarkerStartRendering = true;
}

inline float Entity::GetGUIStartRenderingMarkerTimer()
{
	return myGUIMarkerStartRenderingTime;
}

inline bool Entity::GetGUIStartReneringMarker()
{
	return myGUIMarkerStartRendering;
}