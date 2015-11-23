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

class KillXEnemiesMission : public Mission, public Subscriber
{
public:
	KillXEnemiesMission(Level& aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	~KillXEnemiesMission();

	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory) override;
	void Start() override;
	void End() override;

	void ReceiveMessage(const EnemyKilledMessage& aMessage) override;

private:
	bool operator=(KillXEnemiesMission&) = delete;
	Level& myLevel;
	int myEnemiesToKill;
	int myEnemiesToKillStart;
	bool myActive;
};

