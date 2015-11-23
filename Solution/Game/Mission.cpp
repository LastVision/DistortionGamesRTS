#include "stdafx.h"
#include "Mission.h"
#include "Event.h"
#include "MissionEventReader.h"
#include "PostMaster.h"
#include "EnqueueEventMessage.h"

Mission::Mission(XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: myIndex(-1)
	, myStartEventNames(32)
	, myEndEventNames(32)
{
	MissionEventReader::ReadEventNames(aReader, aElement, myStartEventNames, myEndEventNames);
}


Mission::~Mission()
{
}

void Mission::Start()
{
}

void Mission::EventsStart()
{
	for (int i = 0; i < myStartEventNames.Size(); ++i)
	{
		PostMaster::GetInstance()->SendMessage(EnqueueEventMessage(myStartEventNames[i]));
	}
}

void Mission::End()
{

}

bool Mission::EventsEnd()
{
	for (int i = 0; i < myEndEventNames.Size(); ++i)
	{
		PostMaster::GetInstance()->SendMessage(EnqueueEventMessage(myEndEventNames[i]));
	}

	return myEndEventNames.Size() > 0;
}

int Mission::GetIndex() const
{
	return myIndex;
}

void Mission::SetIndex(int aIndex)
{
	myIndex = aIndex;
}

void Mission::PrintMissionText(const std::string& aText, int aMissionIndex)
{
	Prism::Engine* engine = Prism::Engine::GetInstance();
	CU::Vector2<float> screenCenter(engine->GetWindowSize().x * 0.5f, engine->GetWindowSize().y * 0.5f);

	engine->PrintText(aText, { 35.f, -50.f - aMissionIndex * 40.f }
		, Prism::eTextType::RELEASE_TEXT);
}