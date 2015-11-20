#pragma once
#include <GrowingArray.h>
#include "Mission.h"
#include "Subscriber.h"

class Entity;
class Level;
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class MissionContainer : public Mission, public Subscriber
{
public:
	MissionContainer(Level& aLevel, Entity& aPlayer, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	~MissionContainer();

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aCategory) override;
	void Start() override;
	void End() override;

	void ReceiveMessage(const EventQueueEmptyMessage&) override;
private:

	void SortCopy(CU::GrowingArray<Mission*>& someOut, const CU::GrowingArray<Mission*>& someIn) const;
	void Update(float aDeltaTime, CU::GrowingArray<Mission*>& someMissions, int aOffset, eMissionCategory aCategory);

	CU::GrowingArray<Mission*> myRequiredMissions;
	CU::GrowingArray<Mission*> myOptionalMissions;
	CU::GrowingArray<Mission*> myRequiredActiveMissions;
	CU::GrowingArray<Mission*> myOptionalActiveMissions;
	CU::GrowingArray<Mission*> myEndingMissions;
};

