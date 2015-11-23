#pragma once
#include "Enums.h"
#include "PointLight.h"

namespace Prism
{
	class PointLight;
}
class PointLightComponent : public Component
{
public:
	PointLightComponent(Entity& aEntity);
	~PointLightComponent();

	void Update(float aDeltaTime) override;

	Prism::PointLight* GetPointLight();

	static eComponentType GetType();

private:
	Prism::PointLight* myPointLight;
};

inline eComponentType PointLightComponent::GetType()
{
	return eComponentType::POINT_LIGHT;
}

inline Prism::PointLight* PointLightComponent::GetPointLight()
{
	return myPointLight;
}