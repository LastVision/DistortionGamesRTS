#pragma once
#include "Component.h"
#include "EntityEnum.h"
#include <StaticArray.h>

struct BuildingCompnentData;

class BuildingComponent : public Component
{
public:
	BuildingComponent(Entity& aEntity, BuildingCompnentData& aData);
	~BuildingComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
private:
	void BuildUnit(eEntityType aUnitType);

	CU::StaticArray<eEntityType, 3> myBuildTypes;
};

inline eComponentType BuildingComponent::GetTypeStatic()
{
	return eComponentType::BUILDING;
}

inline eComponentType BuildingComponent::GetType()
{
	return GetTypeStatic();
}
