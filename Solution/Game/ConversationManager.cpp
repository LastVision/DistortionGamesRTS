#include "stdafx.h"
#include "Conversation.h"
#include "ConversationManager.h"
#include <XMLReader.h>

ConversationManager::ConversationManager(const std::string& aXmlPath)
	: myCurrent(nullptr)
{
	XMLReader reader;
	reader.OpenDocument(aXmlPath);
	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	tinyxml2::XMLElement* timeElement = reader.FindFirstChild(element, "conversationTime");
	float conversationTime = 3.f;
	if (timeElement != nullptr)
	{
		reader.ReadAttribute(timeElement, "value", conversationTime);
	}

	for (element = reader.FindFirstChild(element, "conversation"); element != nullptr;
		element = reader.FindNextElement(element, "conversation"))
	{
		std::string name;
		reader.ForceReadAttribute(element, "name", name);
		name = CU::ToLower(name);

		myConversations[name] = new Conversation(name, reader, element, conversationTime);
	}
	reader.CloseDocument();

	//tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "conversation");

	//for (tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "sentence"); element != nullptr;
	//	element = aReader.FindNextElement(aElement, "sentence"))
	//{
	//	std::string text;
	//	aReader.ForceReadAttribute(element, "text", text);

	//	mySentences.Add(new Sentence(text));
	//}
}


ConversationManager::~ConversationManager()
{
}

void ConversationManager::Start(const std::string& aConversation)
{
	std::unordered_map<std::string, Conversation*>::const_iterator it = myConversations.find(aConversation);

	if (it == myConversations.end())
	{
		DL_ASSERT(("Conversation does not exist: " + aConversation).c_str());
	}
	else
	{
		myCurrent = it->second;
		myCurrent->Start();
	}
}

bool ConversationManager::Update()
{
	if (myCurrent != nullptr)
	{
		if (myCurrent->Update() == true)
		{
			myCurrent = nullptr;
			return true;
		}
	}
	return false;
}