#pragma once

enum eOwnerType
{
	NOT_USED = -1,
	PLAYER = 1,
	ENEMY = 2,
	TRIGGER = 4,
};

enum eEntityType
{
	DRAGON,
	DRAGON_STATIC,
	_COUNT,
};

enum class eEntityState : int
{
	IDLE,
	WALKING,
	ATTACKING,
	DYING,
	_COUNT,
};

enum class eComponentType
{
	NOT_USED = -1,
	ACTOR,
	ANIMATION,
	COLLISION,
	CONTROLLER,
	GRAPHICS,
	MOVEMENT,
	_COUNT,
};