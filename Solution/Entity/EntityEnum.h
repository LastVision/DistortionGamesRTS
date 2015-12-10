#pragma once

enum eOwnerType
{
	NOT_USED = -1,
	PLAYER = 1,
	ENEMY = 2,
	NEUTRAL = 4
};

enum eEntityType
{
	EMPTY = -1,
	UNIT = 1,
	PROP = 2,
	BASE_BUILING = 4,
	RESOURCE_POINT = 8,
	VICTORY_POINT = 16,
	DRAGON = 32, // to be moved to UnitType
	DRAGON_STATIC = 64, // to be deleted
	PINE_TREE = 128, // to be moved to propType
	_COUNT,
};

enum class eUnitType
{
	DRAGON,
};

enum class ePropType
{
	PINE_TREE,
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
	CONTROLLER, //Controller HAS to be above Actor in this list!!!
	ACTOR,
	ANIMATION,
	TRIGGER,
	COLLISION,
	GRAPHICS,
	MOVEMENT,
	HEALTH,
	BUILDING,
	_COUNT,
};

enum class eTriggerType
{
	RESOURCE,
};