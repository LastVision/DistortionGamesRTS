#include "stdafx.h"

#include "ActorComponent.h"
#include "ActorComponentData.h"
#include "ControllerComponent.h"
#include <Terrain.h>

ActorComponent::ActorComponent(Entity& aEntity, ActorComponentData& aData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, mySpeed(aData.myMoveSpeed)
{
}

ActorComponent::~ActorComponent()
{
}

void ActorComponent::Update(float aDelta)
{
	const ControllerComponent::ControllerData& data = myEntity.GetComponent<ControllerComponent>()->GetControllerData();

	CU::Vector3<float> direction = data.myDirection;
	if (CU::Length(direction) > 0.f)
	{
		CU::Normalize(direction);
		LookInDirection(direction);
		myEntity.SetState(eEntityState::WALKING);
		
		CU::Vector3<float> position = myEntity.myOrientation.GetPos();

		position += direction * mySpeed * aDelta;
		myEntity.myOrientation.SetPos(position);
		myTerrain.CalcEntityHeight(myEntity.myOrientation);
	}
}

void ActorComponent::LookInDirection(const CU::Vector3<float>& aDirection)
{
	CU::Vector3<float> direction = CU::GetNormalized(aDirection);

	CU::Vector3<float> forward = myEntity.myOrientation.GetForward();
	CU::Normalize(forward);

	if (forward != direction)
	{
		CU::Vector3<float> planeNormal(0, 1.f, 0);
		float dot = CU::Dot(forward, direction);
		float det = CU::Dot(planeNormal, CU::Cross(forward, direction));

		float angle = atan2(det, dot);

		CU::Vector3<float> pos = myEntity.myOrientation.GetPos();
		myEntity.myOrientation.SetPos({ 0.f, 0.f, 0.f });
		myEntity.myOrientation = myEntity.myOrientation * CU::Matrix44<float>::CreateRotateAroundY(angle);
		myEntity.myOrientation.SetPos(pos);
	}
}

void ActorComponent::LookAtPoint(const CU::Vector3<float>& aPoint)
{
	CU::Vector3<float> direction = aPoint - myEntity.GetOrientation().GetPos();
	LookInDirection(direction);
}