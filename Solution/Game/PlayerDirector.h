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

namespace CU
{
	namespace Intersection
	{
		class LineSegment3D;
	}
}

class BuildingComponent;

class PlayerDirector : public Director
{
public:
	PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor);
	~PlayerDirector();

	void InitGUI();

	void Update(float aDeltaTime, const Prism::Camera& aCamera);
	void Render(const Prism::Camera& aCamera);

	void OnResize(int aWidth, int aHeight);

	void SpawnUnit(Prism::Scene& aScene);

	void SelectUnit(Entity* anEntity);

	void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const ToggleGUIMessage& aMessage) override;

	const CU::GrowingArray<Entity*>& GetSelectedUnits() const;
	const BuildingComponent& GetBuildingComponent() const;
	CU::Vector3<float> GetCameraMoveVector() const;
private:
	void UpdateInputs();
	CU::Vector3<float> CalcCursorWorldPosition(const Prism::Camera& aCamera);
	void UpdateMouseInteraction(const Prism::Camera& aCamera);
	void SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
		, const CU::Intersection::LineSegment3D& aMouseRay);

	CU::GrowingArray<Entity*> mySelectedUnits;
	GUI::GUIManager* myGUIManager;
	GUI::Cursor* myCursor;

	bool myShiftPressed;
	bool myAPressed;
	bool mySPressed;
	bool myLeftMouseClicked;
	bool myRightClicked;

	bool myRenderGUI;

	float myTweakValueX;
	float myTweakValueY;
};

inline const CU::GrowingArray<Entity*>& PlayerDirector::GetSelectedUnits() const
{
	return mySelectedUnits;
}