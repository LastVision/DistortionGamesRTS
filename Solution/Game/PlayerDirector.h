#pragma once
#include "Director.h"

namespace Prism
{
	class Camera;
	class Terrain;
	class Scene;
	class SpriteProxy;
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
class AIDirector;

class PlayerDirector : public Director
{
public:
	PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor);
	~PlayerDirector();

	void InitGUI(const AIDirector* anAI, const Prism::Camera& aCamera);

	void Update(float aDeltaTime, const Prism::Camera& aCamera);
	void Render(const Prism::Camera& aCamera);

	void OnResize(int aWidth, int aHeight);

	void SelectUnit(Entity* anEntity);

	void ReceiveMessage(const ToggleGUIMessage& aMessage) override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;
	void ReceiveMessage(const TimeMultiplierMessage& aMessage) override;
	void ReceiveMessage(const MinimapMoveMessage& aMessage) override;
	void ReceiveMessage(const ToggleBuildTimeMessage& aMessage) override;

	const CU::GrowingArray<Entity*>& GetSelectedUnits() const;
	const BuildingComponent& GetBuildingComponent() const;
	CU::Vector3<float> GetCameraMoveVector() const;

	const int& GetTestGold() const;
	const bool& GetRenderDragSelection() const;

private:
	void UpdateInputs();
	CU::Vector3<float> CalcCursorWorldPosition(const CU::Vector2<float>& aMousePosition, const Prism::Camera& aCamera);
	void UpdateMouseInteraction(const Prism::Camera& aCamera);
	void SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
		, const CU::Intersection::LineSegment3D& aMouseRay);
	void SelectAllUnits();

	CU::GrowingArray<Entity*> mySelectedUnits;
	GUI::GUIManager* myGUIManager;
	GUI::Cursor* myCursor;

	bool myShiftPressed;
	bool myLeftMouseUp;
	bool myLeftMouseDown;
	bool myLeftMousePressed;
	bool myRightClicked;
	bool myMouseIsOverGUI;
	bool myRenderDragSelection;
	eSelectedAction mySelectedAction;

	bool myRenderGUI;

	float myTweakValueX;
	float myTweakValueY;

	CU::Vector2<float> mySelectionSpriteSize;
	CU::Vector2<float> mySelectionSpriteRenderPosition;
	CU::Vector2<float> mySelectionSpriteHotspot;

	CU::Vector2<float> myFirstMousePosition;
	CU::Vector3<float> myFirstCameraPosition;
	CU::Vector3<float> myFirstMousePositionInWorld;
	CU::GrowingArray<CU::Vector3<float>> myDragSelectionPositions;
	Prism::SpriteProxy* myDragSelectionSprite;


};

inline const CU::GrowingArray<Entity*>& PlayerDirector::GetSelectedUnits() const
{
	return mySelectedUnits;
}

inline const int& PlayerDirector::GetTestGold() const
{
	return myTestGold;
}

inline const bool& PlayerDirector::GetRenderDragSelection() const
{
	return myRenderDragSelection;
}