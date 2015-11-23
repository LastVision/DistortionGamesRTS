#pragma once
#include "Enums.h"

struct MissionNote
{
	MissionNote(eMissionType aType, eMissionEvent aEvent);

	const eMissionType myType;
	const eMissionEvent myEvent;

private:
	void operator=(MissionNote&) = delete;
};

inline MissionNote::MissionNote(eMissionType aType, eMissionEvent aEvent)
	: myType(aType)
	, myEvent(aEvent)
{
}