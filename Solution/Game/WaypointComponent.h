#pragma once
#include "Component.h"

class WaypointComponent : public Component
{
public:
	WaypointComponent(Entity& aEntity);

	void ReceiveNote(const CollisionNote& aNote) override;

	static eComponentType GetType();

private:
};

inline eComponentType WaypointComponent::GetType()
{
	return eComponentType::WAY_POINT;
}