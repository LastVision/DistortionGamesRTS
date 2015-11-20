#pragma once

#include "Message.h"

class Entity;

class BulletCollisionToGUIMessage : public Message
{
public:
	BulletCollisionToGUIMessage(const Entity& aBullet, const Entity& aEntityCollidedWith, eEntityType aBulletOwner);

	const Entity& myBullet;
	const Entity& myEntityCollidedWith;
	const eEntityType myBulletOwner;
};


inline BulletCollisionToGUIMessage::BulletCollisionToGUIMessage(const Entity& aBullet, const Entity& aEntityCollidedWith
		, eEntityType aBulletOwner)
	: myBullet(aBullet)
	, myEntityCollidedWith(aEntityCollidedWith)
	, myBulletOwner(aBulletOwner)
	, Message(eMessageType::BULLET_COLLISION_TO_GUI)
{
}
