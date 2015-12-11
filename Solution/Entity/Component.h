#pragma once
#include <EntityEnum.h>

class Entity;

class Component
{
public:
	Component(Entity& aEntity);
	virtual ~Component();

	virtual void Init();
	virtual void Update(float aDeltaTime);

	Entity& GetEntity();

	static eComponentType GetTypeStatic();
	virtual eComponentType GetType();

	virtual void Reset();

protected:
	void operator=(Component&) = delete;

	Entity& myEntity;
};

inline eComponentType Component::GetTypeStatic()
{
	return eComponentType::NOT_USED;
}

inline eComponentType Component::GetType()
{
	return GetTypeStatic();
}


inline Entity& Component::GetEntity()
{
	return myEntity;
}