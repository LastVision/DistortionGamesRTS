#pragma once

enum eOwnerType
{
	NOT_USED = -1,
	PLAYER = 1,
	ENEMY = 2,
	TRIGGER = 4,
	NEUTRAL = 8
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
	GRAPHICS = 0,
	ANIMATION,
	MOVEMENT,
	COLLISION,
	_COUNT,
};