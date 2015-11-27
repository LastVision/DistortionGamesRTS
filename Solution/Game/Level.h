#pragma once

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Scene;
	class Terrain;
}

class Entity;

class Level
{
public:
	Level(const Prism::Camera& aCamera);
	~Level();

	bool LogicUpdate(float aDeltaTime, Prism::Camera& aCamera);
	void Render();

	void OnResize(int aWidth, int aHeigth);

private:
	CU::Vector3<float> CalcCursorWorldPosition(Prism::Camera& aCamera);

	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	CU::GrowingArray<Entity*> myUnits;
	CU::GrowingArray<CU::Vector3<float>> myWaypoints;
	int myCurrentWaypoint;
};

