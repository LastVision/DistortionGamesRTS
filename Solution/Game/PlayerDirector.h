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

enum class eSelectedAction
{
	NONE,
	MOVE,
	ATTACK_TAGRET,
	STOP,
	ATTACK_MOVE,
	HOLD_POSITION,
	PATROL
};

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
	void ReceiveMessage(const OnClickMessage& aMessage) override;
	void ReceiveMessage(const ResourceMessage& aMessage) override;
	void ReceiveMessage(const TimeMultiplierMessage& aMessage) override;

	const CU::GrowingArray<Entity*>& GetSelectedUnits() const;
	const BuildingComponent& GetBuildingComponent() const;
	CU::Vector3<float> GetCameraMoveVector() const;

	const int& GetTestGold() const;
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
	bool myLeftMouseClicked;
	bool myRightClicked;
	eSelectedAction mySelectedAction;

	bool myRenderGUI;

	float myTweakValueX;
	float myTweakValueY;

	int myTestGold;
};

inline const CU::GrowingArray<Entity*>& PlayerDirector::GetSelectedUnits() const
{
	return mySelectedUnits;
}

inline const int& PlayerDirector::GetTestGold() const
{
	return myTestGold;
}