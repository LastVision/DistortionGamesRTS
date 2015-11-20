#include "stdafx.h"
#include "Conversation.h"
#include "ConversationMessage.h"
#include "PostMaster.h"
#include <TimerManager.h>
#include <XMLReader.h>

Conversation::Conversation(const std::string& aName, XMLReader& aReader, tinyxml2::XMLElement* aElement
		, float aConversationTime)
	: mySentences(16)
	, myShowTime(aConversationTime)
	, myName(aName)
{
#ifdef _DEBUG
	myShowTime = 0.1f;
#endif

	for (tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "sentence");
		element != nullptr; element = aReader.FindNextElement(element, "sentence"))
	{
		std::string text;
		aReader.ForceReadAttribute(element, "text", text);
		if (text == "")
		{
			text = " ";
		}
		mySentences.Add(new Sentence(text));
	}
}

Conversation::~Conversation()
{
	mySentences.DeleteAll();
}

void Conversation::Start()
{
	myCurrentSentence = 0;
	myTimeLeft = myShowTime;
	SetCurrentSentence();
}

bool Conversation::Update()
{
	myTimeLeft -= CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();

	if (myTimeLeft < 0)
	{
		myTimeLeft = myShowTime;
		++myCurrentSentence;
		if (myCurrentSentence >= mySentences.Size())
		{
			myCurrentSentence = -1;
			SetCurrentSentence();
			return true;
		}
		SetCurrentSentence();
	}
	return false;
}

void Conversation::SetCurrentSentence()
{
	if (myCurrentSentence == -1)
	{
		PostMaster::GetInstance()->SendMessage<ConversationMessage>(ConversationMessage(" "));
	}
	else
	{
		PostMaster::GetInstance()->SendMessage<ConversationMessage>(
			ConversationMessage(mySentences[myCurrentSentence]->myText));
	}
}