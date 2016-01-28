#pragma once

struct SelectionComponentData
{
	bool myExistsInEntity = false;
	const char* mySelectedPath;
	const char* myHoveredPath;
	const char* myEffectPath;
};