#pragma once
#include <Vector.h>
#include "Enums.h"

struct SoundNote
{
	SoundNote(eSoundNoteType aType, const std::string& aEventName);

	const eSoundNoteType myType;
	const std::string myEventName;

private:
	void operator=(SoundNote&) = delete;
};

inline SoundNote::SoundNote(eSoundNoteType aType, const std::string& aEventName)
	: myType(aType)
	, myEventName(aEventName)
{
}