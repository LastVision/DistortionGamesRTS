#pragma once

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Scene;
	class Terrain;
}

namespace GUI
{
	class Cursor;
}

class AIDirector;
class Entity;
class PlayerDirector;

class Level
{
	friend class LevelFactory;
public:
	Level(const Prism::Camera& aCamera, Prism::Terrain* aTerrain, GUI::Cursor* aCursor);
	~Level();

	bool Update(float aDeltaTime, Prism::Camera& aCamera);
	void Render();

	void OnResize(int aWidth, int aHeigth);

	void SpawnUnit();

	bool HasPlayerWon() const;
	bool HasAIWon() const;

private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	CU::GrowingArray<Entity*> myEntities;
	PlayerDirector* myPlayer;
	AIDirector* myAI;
};
