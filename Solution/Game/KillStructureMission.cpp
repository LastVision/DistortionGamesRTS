#include "stdafx.h"
#include "KillStructureMessage.h"
#include "KillStructureMission.h"
#include "Engine.h"
#include "Entity.h"
#include "PostMaster.h"
#include <sstream>
#include <XMLReader.h>


KillStructureMission::KillStructureMission(XMLReader& aReader, tinyxml2::XMLElement* aElement)
	: Mission(aReader, aElement)
	, myStructuresToKillNames(4)
	, myStructuresToKill(4)
	, myAmountOfStructures(-1)
{
	std::string structureName;
	for (tinyxml2::XMLElement* element = aReader.ForceFindFirstChild(aElement, "structure"); element != nullptr;
		element = aReader.FindNextElement(element, "structure"))
	{
		aReader.ForceReadAttribute(element, "structureName", structureName);
		structureName = CU::ToLower(structureName);
		myStructuresToKillNames.Add(structureName);
	}
	myAmountOfStructures = myStructuresToKillNames.Size();

}

KillStructureMission::~KillStructureMission()
{
	if (PostMaster::GetInstance()->IsSubscribed(eMessageType::KILL_STRUCTURE, this) == true)
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::KILL_STRUCTURE, this);
	}
}

void KillStructureMission::Start()
{
	PostMaster::GetInstance()->Subscribe(eMessageType::KILL_STRUCTURE, this);
	for (int i = 0; i < myStructuresToKillNames.Size(); ++i)
	{
		PostMaster::GetInstance()->SendMessage<KillStructureMessage>(KillStructureMessage(KillStructureMessage::eType::NAME, myStructuresToKillNames[i]));
	}
}

bool KillStructureMission::Update(float, int aMissionIndex, eMissionCategory aMissionCategory)
{
	for (int i = myStructuresToKill.Size() - 1; i >= 0; --i)
	{
		PostMaster::GetInstance()->SendMessage<KillStructureMessage>(KillStructureMessage(KillStructureMessage::eType::TO_GUI, "", myStructuresToKill[i]));
		if (myStructuresToKill[i]->GetAlive() == false)
		{
			myStructuresToKill.RemoveCyclicAtIndex(i);
		}
	}

	std::stringstream ss;
	ss.precision(2);

	int structuresKilled = myAmountOfStructures - myStructuresToKill.Size();

	ss << "Destroy structures " << structuresKilled << "/" << myAmountOfStructures;

	if (aMissionCategory == eMissionCategory::NOT_REQUIRED)
	{
		ss << " (Optional)";
	}

	PrintMissionText(ss.str(), aMissionIndex);

	return myStructuresToKill.Size() <= 0;
}

void KillStructureMission::End()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::KILL_STRUCTURE, this);
}

void KillStructureMission::ReceiveMessage(const KillStructureMessage& aMessage)
{
	if (aMessage.myType == KillStructureMessage::eType::ENTITY)
	{
		myStructuresToKill.Add(aMessage.myEntity);
	}
}