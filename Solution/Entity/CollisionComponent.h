#pragma once

#include "Component.h"
struct CollisionComponentData;

class CollisionComponent : public Component
{
public:
	CollisionComponent(Entity& aEntity, CollisionComponentData& aData);
	~CollisionComponent();

private:
	float myRadius;
};

