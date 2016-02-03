#pragma once
#include <Subscriber.h>

class PlayerDirector;
class NeutralDirector;

class Tutorial : public Subscriber
{
public:
	Tutorial(const std::string& aXMLPath, const PlayerDirector* aPlayer, const CU::Matrix44<float>& aCameraOrientation
		, const NeutralDirector* aNeutral);
	~Tutorial();

	void Update(float aDeltaTime);

	void ReceiveMessage(const TutorialMessage& aMessage) override;

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
		Mission(Prism::Text* aText, Action anAction, float aTime) : myText(aText), myAction(anAction), myTime(aTime) {}
		Prism::Text* myText;
		Action myAction;
		float myTime;
	};

	Action GetAction(const std::string& anAction) const;

	void operator=(Tutorial&) = delete;

	const PlayerDirector* myPlayer;
	const NeutralDirector* myNeutral;
	int myPreviousNeutralCount;
	const CU::Matrix44<float>& myCameraOrientation;
	CU::Vector3<float> myPreviousCameraPosition;

	float myMaxTime;
	float myCurrentTime;

	CU::GrowingArray<Mission> myMissions;
	int myCurrentMission;
	bool myMissionComplete;
	bool myActive;
};

