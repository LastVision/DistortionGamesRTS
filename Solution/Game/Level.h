#pragma once

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Scene;
	class Terrain;
}

class Entity;
class PlayerDirector;

class Level
{
public:
	Level(const Prism::Camera& aCamera);
	~Level();

	bool Update(float aDeltaTime, Prism::Camera& aCamera);
	void Render();

	void OnResize(int aWidth, int aHeigth);

private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	PlayerDirector* myPlayer;
};