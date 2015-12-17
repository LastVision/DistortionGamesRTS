#pragma once
#include "Component.h"
#include "EnrageComponentData.h"
class EnrageComponent :
	public Component
{
public:
	EnrageComponent(Entity& anEntity, EnrageComponentData& aData);
	~EnrageComponent();

	void Update(float aDelta);

	bool IsActive();

private:
	float myDuration;
	float myCooldown;
	bool myIsActive;

};

inline bool EnrageComponent::IsActive()
{
	return myIsActive;
}