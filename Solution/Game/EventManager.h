#pragma once
#include <string>
#include "Subscriber.h"
#include <unordered_map>

class Event;
class Entity;
class ConversationManager;

class EventManager : public Subscriber
{
public:
	EventManager(const std::string& aXmlPath, ConversationManager& aConversationManager
		, Entity& aPlayer);
	~EventManager();

	void ReceiveMessage(const EnqueueEventMessage& aMessage) override;
	
	void Update();
private:
	void operator=(const EventManager&) = delete;
	std::unordered_map<std::string, Event*> myEvents;
	CU::GrowingArray<Event*> myEventQueue;
	Entity& myPlayer;
};

