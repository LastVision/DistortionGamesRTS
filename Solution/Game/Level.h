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

	bool LogicUpdate(float aDeltaTime);
	void Render();

	void OnResize(int aWidth, int aHeigth);

private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	Entity* myUnit;
	Entity* myStaticUnit;
	Entity* myDragon;
};

