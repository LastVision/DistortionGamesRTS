#pragma once
#include "Mission.h"
#include "Subscriber.h"

class Entity;
class Level;
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class KillXEnemiesAbortMission : public Mission, public Subscriber
{
public:
	KillXEnemiesAbortMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	~KillXEnemiesAbortMission();

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory) override;
	void Start() override;
	void End() override;

	void ReceiveMessage(const EnemyKilledMessage& aMessage) override;

private:
	bool operator=(KillXEnemiesAbortMission&) = delete;
	Level& myLevel;
	int myEnemiesToKillVisual;
	int myEnemiesToKillVisualStart;
	int myEnemiesToKillReal;
	int myEnemiesToKillRealStart;
	bool myActive;
};

