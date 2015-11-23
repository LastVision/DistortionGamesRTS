#pragma once

class Entity;
class CollisionManager;

struct CollisionNote
{
	CollisionNote(Entity& anEntity, CollisionManager& aCollisionManager);
	void operator=(CollisionNote&) = delete;

	Entity& myEntity;
	CollisionManager& myCollisionManager;
};

inline CollisionNote::CollisionNote(Entity& anEntity, CollisionManager& aCollisionManager)
	: myEntity(anEntity)
	, myCollisionManager(aCollisionManager)
{
}