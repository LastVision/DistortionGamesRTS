#pragma once
#include <Vector.h>
#include "Enums.h"

class Entity;

struct GUINote
{
	GUINote(Entity* anEntity, eGUINoteType aType);
	GUINote(CU::Vector3<float> aPosition, eGUINoteType aType);
	GUINote(bool anIsHoming, eGUINoteType aType);

	const CU::Vector3<float> myPosition;
	const eGUINoteType myType;
	const bool myIsHoming;
	Entity* myEntity;

private:
	void operator=(GUINote&) = delete;
};

inline GUINote::GUINote(Entity* anEntity, eGUINoteType aType)
	: myType(aType)
	, myEntity(anEntity)
	, myIsHoming(false)
{
}

inline GUINote::GUINote(CU::Vector3<float> aPosition, eGUINoteType aType)
	: myType(aType)
	, myEntity(nullptr)
	, myPosition(aPosition)
	, myIsHoming(false)
{
}

inline GUINote::GUINote(bool anIsHoming, eGUINoteType aType)
	: myType(aType)
	, myEntity(nullptr)
	, myPosition({0.f,0.f,0.f})
	, myIsHoming(anIsHoming)
{
}