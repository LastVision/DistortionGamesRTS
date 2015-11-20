#include "stdafx.h"
#include "InputComponent.h"
#include "ControllerComponent.h"
#include "Entity.h"
#include "GUIComponent.h"
#include "PhysicsComponent.h"
#include "SoundNote.h"
#include "ShieldComponent.h"

PhysicsComponent::PhysicsComponent(Entity& aEntity)
	: Component(aEntity)
	, myVelocity(0, 0, 0)
	, mySpeed(0)
{
}

void PhysicsComponent::Init(const CU::Matrix44<float>& anOrientation, const CU::Vector3<float>& aVelocity, int aWeight)
{
	SetVelocity(aVelocity);
	myWeight = aWeight;
	myEntity.myOrientation = anOrientation;
}

void PhysicsComponent::Init(int aWeight, const CU::Vector3<float>& aVelocity)
{
	SetVelocity(aVelocity);
	myWeight = aWeight;
}

void PhysicsComponent::Update(float aDeltaTime)
{
	myPreviousOrientation2 = myPreviousOrientation; // needed to restore 2 frames back with prop collision
	myPreviousOrientation = myEntity.myOrientation;
	myEntity.myOrientation.SetPos(myEntity.myOrientation.GetPos() + aDeltaTime * myVelocity);
	
	// object loses its speed, velocity approaches 0 every update
	myVelocity.x -= myVelocity.x * (aDeltaTime / 10);
	myVelocity.y -= myVelocity.y * (aDeltaTime / 10);
	myVelocity.z -= myVelocity.z * (aDeltaTime / 10);
}

void PhysicsComponent::Accelerate(float anAcceleration)
{
	mySpeed += anAcceleration;
	myVelocity = myEntity.myOrientation.GetForward() * mySpeed;
}

void PhysicsComponent::Reset()
{
	myVelocity.x = 0.f;
	myVelocity.y = 0.f;
	myVelocity.z = 0.f;
	mySpeed = 0.f;
}

void PhysicsComponent::BounceOff(Entity& anOtherEntity)
{
	CU::Vector3<float> toOther = anOtherEntity.myOrientation.GetPos() - myEntity.myOrientation.GetPos();
	CU::Normalize(toOther);

	if (myEntity.GetType() == eEntityType::PLAYER)
	{
		myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_AsteroidCrash"));
		if (myEntity.GetComponent<ShieldComponent>()->GetCurrentShieldStrength() > 0.f)
		{
			myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_ShieldHit"));
		}
		else
		{
			myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_PlayerHit"));
		}
	}
	
	if (CU::Dot(myEntity.myOrientation.GetForward(), toOther) > 0)
	{
		mySpeed = -10.f;
	}
	else
	{
		mySpeed = 10.f;
	}

	myVelocity = myEntity.myOrientation.GetForward() * mySpeed;
	myEntity.myOrientation = myPreviousOrientation2;
	myEntity.GetComponent<InputComponent>()->SetSkyPosition();

	if (myEntity.GetComponent<GUIComponent>() != nullptr)
	{
		myEntity.GetComponent<GUIComponent>()->SetCockpitOrientation();
	}
}

void PhysicsComponent::Bounce()
{
	mySpeed = -10.f;
	myVelocity = myEntity.myOrientation.GetForward() * mySpeed;
	myEntity.myOrientation = myPreviousOrientation2;
	myEntity.GetComponent<InputComponent>()->SetSkyPosition();
}