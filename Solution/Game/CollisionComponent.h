#pragma once
#include "Component.h"
#include "Enums.h"
#include <Sphere.h>

typedef CU::Intersection::Sphere Sphere;

class CollisionComponent : public Component
{
public:
	CollisionComponent(Entity& aEntity);
	virtual ~CollisionComponent();
	virtual void Init(float someRadius);
	virtual void Update(float aDeltaTime) override;

	void SetCollisionRadius(float someRadius = 0);
	Sphere GetSphere();

	virtual bool CheckCollision(CollisionComponent* anOtherComponent);

	static eComponentType GetType();
	eCollisionType GetCollisionType() const;

protected:
	eCollisionType myCollisionType;

private:
	Sphere mySphere;
};



inline eComponentType CollisionComponent::GetType()
{
	return eComponentType::COLLISION;
}

inline void CollisionComponent::SetCollisionRadius(float someRadius)
{
	mySphere.myRadius = someRadius;
	mySphere.myRadiusSquared = mySphere.myRadius * mySphere.myRadius;
}

inline Sphere CollisionComponent::GetSphere()
{
	return mySphere;
}

inline eCollisionType CollisionComponent::GetCollisionType() const
{
	return myCollisionType;
}