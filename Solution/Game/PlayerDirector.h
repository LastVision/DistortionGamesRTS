#pragma once
#include "Director.h"

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

class PlayerDirector : public Director
{
public:
	PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor);
	~PlayerDirector();

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

