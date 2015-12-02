#pragma once
#include "Actor.h"

namespace Prism
{
	class Camera;
	class Terrain;
	class Scene;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class PlayerActor :	public Actor
{
public:
	PlayerActor(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor);
	~PlayerActor();

	void Update(float aDeltaTime, const Prism::Camera& aCamera);
	void Render();

	void OnResize(int aWidth, int aHeight);

private:
	CU::Vector3<float> CalcCursorWorldPosition(const Prism::Camera& aCamera);
	void UpdateMouseInteraction(const Prism::Camera& aCamera);

	GUI::GUIManager* myGUIManager;
	GUI::Cursor* myCursor;

	bool myRenderGUI;
};

