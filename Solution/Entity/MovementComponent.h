#pragma once
#include "Component.h"
#include <Vector.h>

struct MovementComponentData;

class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, MovementComponentData& aComponentData);
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