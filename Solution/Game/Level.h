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

	bool LogicUpdate(float aDeltaTime, Prism::Camera& aCamer);
	void Render();

	void OnResize(int aWidth, int aHeigth);

private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	CU::GrowingArray<Entity*> myUnits;
};

