#pragma once
#include <Subscriber.h>

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

class Level : public Subscriber
{
	friend class LevelFactory;
public:
	Level(const Prism::Camera& aCamera, Prism::Terrain* aTerrain, GUI::Cursor* aCursor);
	~Level();

	bool Update(float aDeltaTime, Prism::Camera& aCamera);
	void Render(Prism::Camera& aCamera);

	void OnResize(int aWidth, int aHeigth);

	void ReceiveMessage(const LUAToggleRenderLinesMessage& aMessage) override;

	void SpawnUnit();

	bool HasPlayerWon() const;
	bool HasAIWon() const;

	CU::Vector3<float> GetCameraMoveVector() const;
private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	CU::GrowingArray<Entity*> myEntities;
	PlayerDirector* myPlayer;
	AIDirector* myAI;

	bool myRenderNavMeshLines;
};
