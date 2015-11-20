#pragma once

struct EmitterNote
{
	enum class eType
	{
		BULLET
	};

	EmitterNote(eType aType, bool aIsActive, bool aUseEmitterLife);

	
	eType myType;
	bool myIsActive;
	bool myShouldLive;

private:

	void operator=(EmitterNote&) = delete;

};

inline EmitterNote::EmitterNote(eType aType, bool aIsActive, bool aUseEmitterLife)
	: myType(aType)
	, myIsActive(aIsActive)
	, myShouldLive(aUseEmitterLife)
{
}