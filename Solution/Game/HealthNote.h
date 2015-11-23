#pragma once

struct HealthNote
{
	HealthNote(int someHealth, int someMaxHealth);

	int myHealth;
	int myMaxHealth;


private:

	void operator=(HealthNote&) = delete;
};

inline HealthNote::HealthNote(int someHealth, int someMaxHealth)
	: myHealth(someHealth)
	, myMaxHealth(someMaxHealth)
{
}