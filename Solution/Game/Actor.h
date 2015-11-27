#pragma once

class Entity;

enum class eActorType
{
	PLAYER1,
	AI,
	COUNT
};


class Actor
{
public:
	Actor();
	virtual ~Actor();

private:
	
	eActorType myActorType;
	CU::GrowingArray<Entity*> myUnits;

};

