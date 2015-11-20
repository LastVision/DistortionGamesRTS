#pragma once
#include <GrowingArray.h>
#include <string>

namespace tinyxml2
{
	class XMLElement;
}

class Event;
class XMLReader;

class Mission
{
public:
	Mission(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	virtual ~Mission();

	virtual bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aCategory) = 0;
	virtual void Start();
	void EventsStart();
	virtual void End();
	bool EventsEnd();

	virtual int GetIndex() const;
	virtual void SetIndex(int aIndex);

protected:
	void PrintMissionText(const std::string& aText, int aMissionIndex);
	int myIndex;
	CU::GrowingArray<std::string> myStartEventNames;
	CU::GrowingArray<std::string> myEndEventNames;
};

