#include "stdafx.h"
#include "ConversationAction.h"
#include "ConversationManager.h"

ConversationAction::ConversationAction(const std::string& aName, ConversationManager& aConversationManager)
	: myName(aName)
	, myConversationManager(aConversationManager)
{
}

ConversationAction::~ConversationAction()
{
}

void ConversationAction::OnEnter()
{
	myConversationManager.Start(myName);
}


void ConversationAction::OnExit()
{
}

bool ConversationAction::Update()
{
	return myConversationManager.Update();
}