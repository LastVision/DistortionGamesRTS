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
	friend class LevelFactory;
public:
	Level(const Prism::Camera& aCamera, Prism::Terrain* aTerrain);
	~Level();

	bool Update(float aDeltaTime, Prism::Camera& aCamera);
	void Render();

	void OnResize(int aWidth, int aHeigth);

private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	CU::GrowingArray<Entity*> myEntities;
	PlayerDirector* myPlayer;
};
