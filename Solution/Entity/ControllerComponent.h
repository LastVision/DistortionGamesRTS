#pragma once
#include "Component.h"

struct ControllerComponentData;
class ControllerComponent : public Component
{
public:
	ControllerComponent(Entity& aEntity, ControllerComponentData& aData);
	~ControllerComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
};

inline eComponentType ControllerComponent::GetTypeStatic()
{
	return eComponentType::CONTROLLER;
}

inline eComponentType ControllerComponent::GetType()
{
	return GetTypeStatic();
}