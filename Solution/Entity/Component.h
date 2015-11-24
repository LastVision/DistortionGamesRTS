#pragma once
#include "EntityEnum.h"

class Entity;

struct CollisionNote;
struct GUINote;
struct InputNote;
struct MissionNote;
struct PowerUpNote;
struct ShootNote;
struct HealthNote;
struct ShieldNote;
struct SoundNote;
struct EmitterNote;
struct EMPNote;

class Component
{
public:
	Component(Entity& aEntity);
	virtual ~Component();

	virtual void Init();
	virtual void Update(float aDeltaTime);

	virtual void ReceiveNote(const CollisionNote& aMessage);
	virtual void ReceiveNote(const InputNote& aMessage);
	virtual void ReceiveNote(const MissionNote& aMessage);
	virtual void ReceiveNote(const PowerUpNote& aNote);
	virtual void ReceiveNote(const ShootNote& aMessage);
	virtual void ReceiveNote(const GUINote& aNote);
	virtual void ReceiveNote(const HealthNote& aNote);
	virtual void ReceiveNote(const ShieldNote& aNote);
	virtual void ReceiveNote(const SoundNote& aNote);
	virtual void ReceiveNote(const EmitterNote& aNote);
	virtual void ReceiveNote(const EMPNote& aNote);

	Entity& GetEntity();

	static eComponentType GetType();

	virtual void Reset();

protected:
	void operator=(Component&) = delete;

	Entity& myEntity;
};

inline eComponentType Component::GetType()
{
	return eComponentType::NOT_USED;
}

inline Entity& Component::GetEntity()
{
	return myEntity;
}