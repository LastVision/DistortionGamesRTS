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
class EmitterManager;
class NeutralDirector;
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

	void ReceiveMessage(const ToggleRenderLinesMessage& aMessage) override;

	void SpawnUnit(eUnitType aUnitType);

	bool HasPlayerWon() const;
	bool HasAIWon() const;

	CU::Vector3<float> GetCameraMoveVector() const;

	PlayerDirector* GetPlayer();
private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	EmitterManager* myEmitterManager;

	CU::GrowingArray<Entity*> myEntities;
	PlayerDirector* myPlayer;
	AIDirector* myAI;
	NeutralDirector* myNeutralDirector;

	int myMaxVictoryPoint;

	bool myRenderNavMeshLines;
};

inline PlayerDirector* Level::GetPlayer()
{
	return myPlayer;
}