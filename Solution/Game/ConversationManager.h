#pragma once
#include <unordered_map>

class Conversation;

class ConversationManager
{
public:
	ConversationManager(const std::string& aXmlPath);
	~ConversationManager();

	void Start(const std::string& aConversation);
	bool Update();
private:

	Conversation* myCurrent;
	std::unordered_map<std::string, Conversation*> myConversations;
};

