#pragma once

struct ShieldNote
{
	ShieldNote(float someShieldStrength, float someMaxShieldStrength);

	float myShieldStrength;
	float myMaxShieldStrength;


private:

	void operator=(ShieldNote&) = delete;
};

inline ShieldNote::ShieldNote(float someShieldStrength, float someMaxShieldStrength)
	: myShieldStrength(someShieldStrength)
	, myMaxShieldStrength(someMaxShieldStrength)
{
}