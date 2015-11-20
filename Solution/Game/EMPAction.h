#pragma once
#include "Action.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace CU
{
	class TimerManager;
}

class XMLReader;

class EMPAction : public Action
{
public:
	EMPAction(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	~EMPAction();

	void OnEnter() override;
	void OnExit() override;
	bool Update() override;

private:
	void operator=(EMPAction&) = delete;

	CU::Vector3<float> myPosition;
};

