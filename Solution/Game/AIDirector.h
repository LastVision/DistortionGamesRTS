#pragma once
#include "Director.h"

namespace Prism
{
	class Camera;
	class Terrain;
	class Scene;
}

class AIDirector : public Director
{
public:
	AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene);
	~AIDirector();

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

	bool myPlayerHasStarted;

	CU::GrowingArray<Entity*> myIdleUnits;
	Entity* mySurviveGatherer;
};

