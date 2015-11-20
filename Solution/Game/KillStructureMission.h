#pragma once
#include "Mission.h"
#include "Subscriber.h"

class Entity;
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class KillStructureMission : public Mission, public Subscriber
{
public:
	KillStructureMission(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	~KillStructureMission();

	void Start() override;
	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory) override;

	void End() override;

	void ReceiveMessage(const KillStructureMessage& aMessage) override;

private:
	bool operator=(KillStructureMission&) = delete;
	CU::GrowingArray<std::string> myStructuresToKillNames;
	CU::GrowingArray<Entity*> myStructuresToKill;
	int myAmountOfStructures;


};

