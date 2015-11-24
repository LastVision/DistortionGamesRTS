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

void Component::Reset()
{
}