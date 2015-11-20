#include "stdafx.h"
#include "PostMaster.h"
#include "WaitAction.h"
#include "SpawnEnemyMessage.h"
#include <XMLReader.h>
#include <TimerManager.h>

WaitAction::WaitAction(XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: myTimerManager(*CU::TimerManager::GetInstance())
{
	aReader.ForceReadAttribute(aElement, "seconds", myStartTime);
}

WaitAction::~WaitAction()
{
}

void WaitAction::OnEnter()
{
	myTime = myStartTime;
}


void WaitAction::OnExit()
{
	myTime = 0.f;
}

bool WaitAction::Update()
{
	myTime -= myTimerManager.GetMasterTimer().GetTime().GetFrameTime();
	return myTime <= 0.f;
}