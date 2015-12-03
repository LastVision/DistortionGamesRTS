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

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void BuildUnit(eEntityType aUnitType);
private:

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
