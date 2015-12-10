#pragma once

enum class eMessageType
{
	GAME_STATE,
	RESIZE,
	ON_CLICK,
	MOVE_CAMERA,
	LUA_MOVE_CAMERA,
	SPAWN_UNIT,
	CINEMATIC,
	FADE,
	TOGGLE_GUI,
	LUA_RUN_SCRIPT,
	COUNT,
};