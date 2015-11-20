#pragma once
#include "Action.h"

namespace tinyxml2
{
	class XMLElement;
}
class SpawnEnemyMessage;
class XMLReader;

class SpawnEnemyAction : public Action
{
public:
	SpawnEnemyAction(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	~SpawnEnemyAction();

	void OnEnter() override;
	void OnExit() override;
	bool Update() override;

private:
	SpawnEnemyMessage* myMessage;
};

