#pragma once

enum eEntityType
{
	NOT_USED = -1,
	PLAYER = 1,
	ENEMY = 2,
	TRIGGER = 4,
};

enum class eComponentType
{
	NOT_USED = -1,
	GRAPHICS = 0,
	_COUNT,
};