#pragma once
#include "Component.h"
#include <Vector.h>
class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, float aMovementSpeed);
	~MovementComponent();

	void MoveTo(CU::Vector2<float> aPointToMoveTo);

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();

private:

	float myMovementSpeed;

};

inline eComponentType MovementComponent::GetTypeStatic()
{
	return eComponentType::MOVEMENT;
}