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

class Entity;
class PlayerActor;

class Level
{
public:
	Level(const Prism::Camera& aCamera, GUI::Cursor* aCursor);
	~Level();

	bool Update(float aDeltaTime, Prism::Camera& aCamera);
	void Render();

	void OnResize(int aWidth, int aHeigth);

private:
	Prism::Terrain* myTerrain;
	Prism::DirectionalLight* myLight;
	Prism::Scene* myScene;

	PlayerActor* myPlayer;
};