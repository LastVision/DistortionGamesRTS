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

class BlockMap;
class DifferenceMap;
class DecisionMap;
class InfluenceMap;
class TensionMap;
class VulnerabilityMap;

class AIDirector : public Director
{
public:
	AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene);
	~AIDirector();

	void RenderMaps(const Prism::Camera& aCamera);
	void Update(float aDeltaTime) override;

	void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const TimeMultiplierMessage& aMessage) override;
	void ReceiveMessage(const BlockMapMessage& aMessage) override;
	void ReceiveMessage(const KillUnitMessage& aMessage) override;

	void LoadAISettings(const std::string& aFilePath);

private:
	struct Action
	{
		Action() {}
		Action(eFuzzyAI aFuzzyAction)
			: myFuzzyAction(aFuzzyAction)
			, myIsDone(false)
		{}
		Action(eFuzzyAI aFuzzyAction, const CU::Vector2<float>& aPosition)
			: myFuzzyAction(aFuzzyAction)
			, myPosition(aPosition)
			, myIsDone(false)
		{}
		eFuzzyAI myFuzzyAction;
		CU::Vector2<float> myPosition;
		bool myIsDone;
	};
	struct AIMaps
	{
		AIMaps();
		~AIMaps();
		InfluenceMap* myInfluenceMap;
		InfluenceMap* myPlayerInfluenceMap;
		InfluenceMap* myNeutralInfluenceMap;
		InfluenceMap* myPlayerNeutralCombinedInfluence;
		InfluenceMap* myGoalMap;
		BlockMap* myBlockMap;
		TensionMap* myTensionMap;
		DifferenceMap* myDifferenceMap;
		VulnerabilityMap* myVulnerabilityMap;
		DecisionMap* myDecisionMap;
		int myInfluenceRenderIndex;
	};
	struct ControlPointAdvisorData
	{
		int myResourceSquadSize;
		int myOptimalGunpowerCount;
		float myGunpowderValue;

		int myVictorySquadSize;
		float myOptimalVictoryPoints;
		float myVictoryPointValue;
	};
	struct UpgradeAdvisorData
	{
		UpgradeAdvisorData():myArtifactHunters(8){}

		int myGruntsKilledBeforeUpgrade;
		int myRangersKilledBeforeUpgrade;
		int myTanksKilledBeforeUpgrade;

		int myGruntKillCount;
		int myRangerKillCount;
		int myTankKillCount;

		float myUpgradeValue;

		CU::GrowingArray<Entity*> myArtifactHunters;
	};

	void UpdateInfluences();
	void UpdateUnitLists();
	void UpdateAdvisors();
	CU::FuzzySet UpdateResourceAdvisor();
	CU::FuzzySet UpdateUpgradeAdvisor();
	CU::FuzzySet* myFuzzySet;
	

	bool UpdateCurrentAction();
	bool FuzzyDecisionDone();
	void InterpretFuzzySet();
	void StartNextAction();
	Action myCurrentAction;
	CU::GrowingArray<Action> myActionQueue;


	void HandleControlPoints(eFuzzyAI aAction);
	void HandleUpgrades(eFuzzyAI aAction);
	void UpdateTakeControlPoints();
	ControlPointAdvisorData myResourceData;
	UpgradeAdvisorData myUpgradeData;



	bool myPlayerHasStarted;

	CU::GrowingArray<Entity*> myIdleUnits;
	CU::GrowingArray<Entity*> myUnitsOnMission;

	AIMaps myMaps;
};

