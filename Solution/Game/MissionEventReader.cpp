#include "stdafx.h"
#include "MissionEventReader.h"
#include <string>
#include <XMLReader.h>

void MissionEventReader::ReadEventNames(XMLReader& aNamesReader, tinyxml2::XMLElement* aNamesElement
	, CU::GrowingArray<std::string>& someStartEventsOut, CU::GrowingArray<std::string>& someEndEventsOut)
{
	struct EventWithIndex
	{
		int myIndex;
		std::string myName;
	};

	CU::GrowingArray<EventWithIndex> startEvents(32);
	CU::GrowingArray<EventWithIndex> endEvents(32);

	for (tinyxml2::XMLElement* element = aNamesReader.FindFirstChild(aNamesElement, "event"); element != nullptr;
		element = aNamesReader.FindNextElement(element, "event"))
	{
		std::string type;
		aNamesReader.ForceReadAttribute(element, "type", type);

		EventWithIndex eventWithIndex;

		aNamesReader.ForceReadAttribute(element, "name", eventWithIndex.myName);
		DL_ASSERT_EXP(eventWithIndex.myName != "", "Error, event type \"" + type + "\" has no name.");

		aNamesReader.ForceReadAttribute(element, "index", eventWithIndex.myIndex);


		if (type == "start")
		{
			startEvents.Add(eventWithIndex);
		}
		else if (type == "end")
		{
			endEvents.Add(eventWithIndex);
		}
		else
		{
			DL_ASSERT("Invalid event type: " + type);
		}
	}
	int currentIndex = 0;
	while (someStartEventsOut.Size() != startEvents.Size())
	{
		int prevIndex = currentIndex;
		for (int i = 0; i < startEvents.Size(); ++i)
		{
			if (startEvents[i].myIndex == currentIndex)
			{
				someStartEventsOut.Add(startEvents[i].myName);
				++currentIndex;
				break;
			}
		}
		DL_ASSERT_EXP(prevIndex == currentIndex - 1, "Start event index " + std::to_string(currentIndex) + " not found.");
	}

	//currentIndex = 0;
	while (someEndEventsOut.Size() != endEvents.Size())
	{
		int prevIndex = currentIndex;
		for (int i = 0; i < endEvents.Size(); ++i)
		{
			if (endEvents[i].myIndex == currentIndex)
			{
				someEndEventsOut.Add(endEvents[i].myName);
				++currentIndex;
				break;
			}
		}
		DL_ASSERT_EXP(prevIndex == currentIndex - 1, "End event index " + std::to_string(currentIndex) + " not found.");
	}
}