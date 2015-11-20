#pragma once
#include <GrowingArray.h>
#include <string>

class Action;

class Event
{
public:
	Event(const std::string& aName, const CU::GrowingArray<Action*>& someActions);
	~Event();

	void Start();
	bool Update();

private:
	void operator=(Event&) = delete;
	const std::string myName;
	CU::GrowingArray<Action*> myActions;
	int myIndex;
};

