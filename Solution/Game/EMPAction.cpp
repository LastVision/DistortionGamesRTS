#include "stdafx.h"

#include "EMPMessage.h"
#include "PostMaster.h"
#include "EMPAction.h"
#include "SpawnEnemyMessage.h"
#include <XMLReader.h>
#include <TimerManager.h>

EMPAction::EMPAction(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "position"), "X", myPosition.x);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "position"), "Y", myPosition.y);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "position"), "Z", myPosition.z);
	myPosition *= 10.f;
}

EMPAction::~EMPAction()
{
}

void EMPAction::OnEnter()
{
	PostMaster::GetInstance()->SendMessage<EMPMessage>(EMPMessage(myPosition, 2.f));
}


void EMPAction::OnExit()
{
}

bool EMPAction::Update()
{
	return true;
}