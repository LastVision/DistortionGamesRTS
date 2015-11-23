#include "stdafx.h"
#include "Action.h"
#include "Event.h"

Event::Event(const std::string& aName, const CU::GrowingArray<Action*>& someActions)
	: myName(aName)
	, myActions(someActions)
{
	DL_ASSERT_EXP(myActions.Size() > 0, "Can't create event with zero actions.");
}

Event::~Event()
{
	myActions.DeleteAll();
}

void Event::Start()
{
	myIndex = 0;
	myActions[myIndex]->OnEnter();
}

bool Event::Update()
{
	if (myActions[myIndex]->Update() == true)
	{
		myActions[myIndex]->OnExit();

		++myIndex;

		if (myIndex < myActions.Size())
		{
			myActions[myIndex]->OnEnter();
		}
		else
		{
			myIndex = 0;
			return true;
		}
	}
	return false;
}