#include "stdafx.h"
#include "Component.h"

Component::Component(Entity& aEntity)
	: myEntity(aEntity)
{
}

Component::~Component()
{
}

void Component::Init()
{
}

void Component::Update(float)
{
}

void Component::ReceiveNote(const CollisionNote&)
{
}

void Component::ReceiveNote(const InputNote&)
{
}

void Component::ReceiveNote(const MissionNote&)
{
}

void Component::ReceiveNote(const PowerUpNote&)
{
}

void Component::ReceiveNote(const ShootNote&)
{
}

void Component::ReceiveNote(const GUINote&)
{
}

void Component::ReceiveNote(const HealthNote&)
{
}

void Component::ReceiveNote(const ShieldNote&)
{
}

void Component::ReceiveNote(const SoundNote&)
{
}

void Component::ReceiveNote(const EmitterNote&)
{
}

void Component::ReceiveNote(const EMPNote&)
{
}

void Component::Reset()
{
}