#pragma once
#include "Component.h"
#include "EnrageComponentData.h"
class EnrageComponent :
	public Component
{
public:
	EnrageComponent(Entity& anEntity, EnrageComponentData& aData);
	~EnrageComponent();

	void Update(float aDeltaTime);

	bool IsActive();
	void Activate();

private:

	const float myOriginalDuration;
	float myCurrentDuration;

	const float myOriginalCooldown;
	float myCurrentCooldown;

	bool myIsActive;

};

inline bool EnrageComponent::IsActive()
{
	return myIsActive;
}