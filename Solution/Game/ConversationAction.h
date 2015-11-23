#pragma once
#include "Action.h"
#include <GrowingArray.h>

namespace tinyxml2
{
	class XMLElement;
}
class ConversationMessage;
class XMLReader;
class ConversationManager;

class ConversationAction : public Action
{
public:
	ConversationAction(const std::string& aName, ConversationManager& aConversationManager);
	~ConversationAction();

	void OnEnter() override;
	void OnExit() override;
	bool Update() override;

private:
	void operator=(ConversationAction&) = delete;
	const std::string myName;
	ConversationManager& myConversationManager;
};

