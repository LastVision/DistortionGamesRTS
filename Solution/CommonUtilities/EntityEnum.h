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
	_COUNT,
};

enum eUnitType
{
	NOT_A_UNIT = -1,
	GRUNT,
	RANGER,
	TANK,
};

enum class ePropType
{
	NOT_A_PROP,
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
	VICTORY,
};
#undef ERROR
enum class eHistoryType
{
	ERROR,
	HISTORY,
	HELP,
	GENERATED_COMMAND,
	WARNING
};