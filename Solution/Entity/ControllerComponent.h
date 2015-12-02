#pragma once
#include "Component.h"
#include <GrowingArray.h>
#include <Vector.h>

namespace Prism
{
	class Terrain;
}

struct ControllerComponentData;
class ControllerComponent : public Component
{
public:
	struct ControllerData
	{
		CU::Vector3<float> myDirection;
	};

	ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain);
	~ControllerComponent();

	void Update(float aDelta) override;

	void MoveTo(const CU::Vector3<float>& aPosition, bool aClearPreviousPoints);

	const ControllerData& GetControllerData() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override; 

private:
	ControllerData myData;
	CU::GrowingArray<CU::Vector3<float>> myWayPoints;
	CU::Vector3<float> myCurrentWayPoint;
	const Prism::Terrain& myTerrain;
};

inline const ControllerComponent::ControllerData& ControllerComponent::GetControllerData() const
{
	return myData;
}

inline eComponentType ControllerComponent::GetTypeStatic()
{
	return eComponentType::CONTROLLER;
}

inline eComponentType ControllerComponent::GetType()
{
	return GetTypeStatic();
}