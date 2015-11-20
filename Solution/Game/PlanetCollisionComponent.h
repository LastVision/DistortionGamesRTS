#pragma once
#include "CollisionComponent.h"

class PlanetCollisionComponent : public CollisionComponent
{
public:
	PlanetCollisionComponent(Entity& aEntity);

	void Init(float someRadius);

	void Update(float aDeltaTime) override;

	bool CheckCollision(CollisionComponent* anOtherComponent) override;


	static eComponentType GetType();

private:
	Sphere myInnerSphere;
	Sphere myOuterSphere;
};

inline eComponentType PlanetCollisionComponent::GetType()
{
	return eComponentType::COLLISION;
}
