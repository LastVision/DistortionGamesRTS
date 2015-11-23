#include "stdafx.h"
#include <Engine.h>
#include "Entity.h"
#include <Intersection.h>
#include "PlanetCollisionComponent.h"

PlanetCollisionComponent::PlanetCollisionComponent(Entity& aEntity)
	: CollisionComponent(aEntity)
{
	myCollisionType = eCollisionType::PLANET;
}

void PlanetCollisionComponent::Init(float)
{
	myOuterSphere.myCenterPosition = myEntity.myOrientation.GetPos();
	myInnerSphere.myCenterPosition = myEntity.myOrientation.GetPos();

	myOuterSphere.myRadius = 1000.f;
	myOuterSphere.myRadiusSquared = myOuterSphere.myRadius * myOuterSphere.myRadius;

	myInnerSphere.myRadius = 1000.f - 50.f;
	myInnerSphere.myRadiusSquared = myInnerSphere.myRadius * myInnerSphere.myRadius;
}

void PlanetCollisionComponent::Update(float)
{
	myOuterSphere.myCenterPosition = myEntity.myOrientation.GetPos();
	myInnerSphere.myCenterPosition = myEntity.myOrientation.GetPos();
}

bool PlanetCollisionComponent::CheckCollision(CollisionComponent* anOtherComponent)
{
	if (CU::Intersection::SphereVsSphere(myOuterSphere, anOtherComponent->GetSphere()) == false)
	{
		Prism::Engine::GetInstance()->PrintText("Completely Outside", { 800, -200 }, Prism::eTextType::DEBUG_TEXT);
		return false;
	}

	if (CU::Intersection::SphereVsSphere(myInnerSphere, anOtherComponent->GetSphere()) == true)
	{
		Prism::Engine::GetInstance()->PrintText("Completely Inside", { 800, -200 }, Prism::eTextType::DEBUG_TEXT);
		return true;
	}

	if (fabsf(anOtherComponent->GetEntity().myOrientation.GetPos().y) > 30.f)
	{
		Prism::Engine::GetInstance()->PrintText("Between, COLLISION", { 800, -200 }, Prism::eTextType::DEBUG_TEXT);
		return true;
	}
	// object inside outer, but outside inner sphere

	Prism::Engine::GetInstance()->PrintText("Between, not collision", { 800, -200 }, Prism::eTextType::DEBUG_TEXT);
	return false;
}