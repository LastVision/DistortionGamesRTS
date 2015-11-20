#pragma once
#include "Mission.h"
#include "Subscriber.h"

class Entity;
class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

class DefendMission : public Mission, public Subscriber
{
public:
	DefendMission(XMLReader& aReader, tinyxml2::XMLElement* aElement, bool aAbortMission);
	~DefendMission();
	
	void Start() override;
	bool Update(float aDeltaTime, int aMissionIndex, eMissionCategory aMissionCategory) override;

	void End() override;

	void ReceiveMessage(const DefendMessage& aMessage) override;

private:
	bool operator=(DefendMission&) = delete;
	std::string myNameToDefend;
	Entity* myEntityToDefend;

	const bool myAbortMission;

	float myRealTime;
	float myRealTimeStart;

	float myVisualTime;
	float myVisualTimeStart;
};

