#include "stdafx.h"

#include "Entity.h"
#include "PointLightComponent.h"
#include <PointLight.h>

PointLightComponent::PointLightComponent(Entity& aEntity)
	: Component(aEntity)
{
	myPointLight = new Prism::PointLight();
	myPointLight->SetColor({ 1, 1, 1, 1 });
	myPointLight->SetRange(200.f);

}


PointLightComponent::~PointLightComponent()
{
	delete myPointLight;
	myPointLight = nullptr;
}

void PointLightComponent::Update(float aDeltaTime)
{
	aDeltaTime;
	myPointLight->SetPosition(GetEntity().myOrientation.GetPos4());
}
