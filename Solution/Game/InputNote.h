#pragma once

struct InputNote
{
	InputNote(unsigned int aKey);

	const unsigned int myKey;

private:
	void operator=(InputNote&) = delete;
};

inline InputNote::InputNote(unsigned int aKey)
	: myKey(aKey)
{
}