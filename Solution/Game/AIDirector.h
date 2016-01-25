#pragma once
#include "Director.h"

namespace CU
{
	class FuzzySet;
}

namespace Prism
{
	class Camera;
	class Terrain;
	class Scene;
}

class DifferenceMap;
class InfluenceMap;
class TensionMap;
class VulnerabilityMap;
class DecisionMap;

class AIDirector : public Director
{
public:
	AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene);
	~AIDirector();

	void RenderMaps(const Prism::Camera& aCamera);
	void Update(float aDeltaTime) override;

	void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const TimeMultiplierMessage& aMessage) override;

private:
	struct Action
	{
		Action() {}
		Action(eFuzzyAI aFuzzyAction)
			: myFuzzyAction(aFuzzyAction)
		{}
		Action(eFuzzyAI aFuzzyAction, const CU::Vector2<float>& aPosition)
			: myFuzzyAction(aFuzzyAction)
			, myPosition(aPosition)
		{}
		eFuzzyAI myFuzzyAction;
		CU::Vector2<float> myPosition;
	};

	void UpdateActionQueue();
	void UpdateAdvisors();
	CU::FuzzySet UpdateAttackAdvisor();
	CU::FuzzySet UpdateDefendAdvisor();
	CU::FuzzySet UpdateResourceAdvisor();
	CU::FuzzySet* myFuzzySet;
	CU::GrowingArray<Action> myActionQueue;

	void ExecuteFuzzyAction();
	bool FuzzyActionDone() const;
	eFuzzyAI myCurrentFuzzyAction;

	//void NotLoseLogic();
	//void WinSlowlyLogic();
	void UpdateInfluences();

	bool myPlayerHasStarted;

	CU::GrowingArray<Entity*> myIdleUnits;
	Entity* mySurviveGatherer;

	InfluenceMap* myInfluenceMap;
	InfluenceMap* myPlayerInfluenceMap;
	InfluenceMap* myNeutralInfluenceMap;
	InfluenceMap* myPlayerNeutralCombinedInfluence;
	InfluenceMap* myGoalMap;
	TensionMap* myTensionMap;
	DifferenceMap* myDifferenceMap;
	VulnerabilityMap* myVulnerabilityMap;
	DecisionMap* myDecisionMap;
	int myInfluenceRenderIndex;

	float myRedistributeUnitsTimer;
	float myCurrentRedistributeUnitsTimer;
};

