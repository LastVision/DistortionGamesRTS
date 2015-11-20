#pragma once

#include <GrowingArray.h>
#include <string>
#include "Subscriber.h"

class Entity;
class EventQueueEmptyMessage;
class Level;
class Mission;

class MissionManager : public Subscriber
{
public:
	MissionManager(Level& aLevel, Entity& aPlayer, const std::string& aFileToReadFrom);
	~MissionManager();
	void Init();
	void Update(float aDeltaTime, bool aForceNextMission);

	void ReceiveMessage(const EventQueueEmptyMessage&) override;
private:
	bool operator=(MissionManager&) = delete;

	Level& myLevel;
	Entity& myPlayer;
	int myCurrentMission;
	CU::GrowingArray<Mission*> myMissions;
	CU::GrowingArray<Mission*> myMissionsNotOrder;

	bool myAllowedToStartNextMission;
	bool myEndEventsActive;
};
