#pragma once
#include "Director.h"

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
	enum class eAction
	{
		CAPTURE_POINT,
		SPAWN_GRUNT,
		SPAWN_RANGER,
		SPAWN_TANK,
		NONE
	};

	void NotLoseLogic();
	void WinSlowlyLogic();
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

