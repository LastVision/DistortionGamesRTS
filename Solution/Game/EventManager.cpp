#include "stdafx.h"
#include "CommonHelper.h"
#include "ConversationAction.h"
#include "EMPAction.h"
#include "Event.h"
#include "EventManager.h"
#include "EventQueueEmptyMessage.h"
#include "InvulnerabilityAction.h"
#include "PostMaster.h"
#include "SpawnEnemyAction.h"
#include "EnqueueEventMessage.h"
#include "WaitAction.h"
#include <XMLReader.h>

EventManager::EventManager(const std::string& aXmlPath, ConversationManager& aConversationManager
		, Entity& aPlayer)
	: myEventQueue(16)
	, myPlayer(aPlayer)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::START_EVENT, this);
	XMLReader reader;
	reader.OpenDocument(aXmlPath);
	std::unordered_map<std::string, int> eventNames;
	tinyxml2::XMLElement* eventElement = reader.ForceFindFirstChild("root");
	
	for (eventElement = reader.FindFirstChild(eventElement, "event"); eventElement != nullptr;
		eventElement = reader.FindNextElement(eventElement, "event"))
	{
		std::string name;
		reader.ForceReadAttribute(eventElement, "name", name);
		name = CU::ToLower(name);

		auto it = eventNames.find(name);
		if (it != eventNames.end())
		{
			DL_ASSERT(name + ": Duplicated eventname in: " + aXmlPath);
		}
		eventNames[name] = 1;

		CU::GrowingArray<Action*> actions(16);

		for (tinyxml2::XMLElement* element = reader.FindFirstChild(eventElement, "spawnEnemy"); element != nullptr;
			element = reader.FindNextElement(element, "spawnEnemy"))
		{
			actions.Add(new SpawnEnemyAction(reader, element));
		}
		for (tinyxml2::XMLElement* element = reader.FindFirstChild(eventElement, "wait"); element != nullptr;
			element = reader.FindNextElement(element, "wait"))
		{
			actions.Add(new WaitAction(reader, element));
		}
		for (tinyxml2::XMLElement* element = reader.FindFirstChild(eventElement, "emp"); element != nullptr;
			element = reader.FindNextElement(element, "emp"))
		{
			actions.Add(new EMPAction(reader, element));
		}
		for (tinyxml2::XMLElement* element = reader.FindFirstChild(eventElement, "invulnerability"); element != nullptr;
			element = reader.FindNextElement(element, "invulnerability"))
		{
			actions.Add(new InvulnerabilityAction(reader, element, myPlayer));
		}

		tinyxml2::XMLElement* element = reader.FindFirstChild(eventElement, "conversation");
		if (element != nullptr)
		{
			actions.Add(new ConversationAction(name, aConversationManager));
		}
		myEvents[name] = new Event(name, actions);
	}
	reader.CloseDocument();
}


EventManager::~EventManager()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::START_EVENT, this);
	for (auto it = myEvents.begin(); it != myEvents.end(); ++it)
	{
		delete it->second;
	}
}

void EventManager::ReceiveMessage(const EnqueueEventMessage& aMessage)
{
	std::string eventName = CU::ToLower(aMessage.GetName());
	std::unordered_map<std::string, Event*>::const_iterator it = myEvents.find(eventName);

	if (it == myEvents.end())
	{
		DL_ASSERT(("Event does not exist: " + eventName).c_str());
	}
	else
	{
		myEventQueue.Add(myEvents[eventName]);
		if (myEventQueue.Size() == 1)
		{
			myEventQueue.GetLast()->Start();
		}
	}
}

void EventManager::Update()
{
	if (myEventQueue.Size() > 0 && myEventQueue[0]->Update() == true)
	{
		myEventQueue.RemoveNonCyclicAtIndex(0);
		
		if (myEventQueue.Size() > 0)
		{
			myEventQueue[0]->Start();
		}
		else
		{
			PostMaster::GetInstance()->SendMessage<EventQueueEmptyMessage>(EventQueueEmptyMessage());
		}
	}
}