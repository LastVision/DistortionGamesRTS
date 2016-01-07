#pragma once

class Director;

class Tutorial
{
public:
	Tutorial(const std::string& aXMLPath, const Director* aPlayer, const CU::Matrix44<float>& aCameraOrientation);
	~Tutorial();

	void Update(float aDeltaTime);

private:

	enum class Action
	{
		CLICK,
		ATTACK,
		SELECT,
		MOVE_CAMERA,
	};

	struct Mission
	{
		Mission() {}
		Mission(const std::string& aText, Action anAction) : myText(aText), myAction(anAction) {}
		std::string myText;
		Action myAction;
	};

	void operator=(Tutorial&) = delete;

	const Director* myPlayer;
	const CU::Matrix44<float>& myCameraOrientation;
	CU::Vector3<float> myPreviousCameraPosition;

	float myMaxTime;
	float myCurrentTime;

	CU::GrowingArray<Mission> myMissions;
	int myCurrentMission;
	bool myMissionComplete;
	bool myActive;
};

