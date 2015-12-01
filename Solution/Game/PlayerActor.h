#pragma once
#include "Actor.h"

namespace Prism
{
	class Camera;
	class Terrain;
	class Scene;
}

class PlayerActor :	public Actor
{
public:
	PlayerActor(const Prism::Terrain& aTerrain, Prism::Scene& aScene);
	~PlayerActor();

	void Update(float aDeltaTime, const Prism::Camera& aCamera);

private:
	CU::Vector3<float> CalcCursorWorldPosition(const Prism::Camera& aCamera);
	void UpdateMouseInteraction(const Prism::Camera& aCamera);
};

