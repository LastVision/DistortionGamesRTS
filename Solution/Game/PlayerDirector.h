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

class PlayerDirector : public Director
{
public:
	PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor);
	~PlayerDirector();

	void Update(float aDeltaTime, const Prism::Camera& aCamera);
	void Render();

	void OnResize(int aWidth, int aHeight);

	void SpawnUnit(Prism::Scene& aScene);

	void SelectUnit(Entity* anEntity);

	void ReceiveMessage(const SpawnUnitMessage& aMessage) override;

	const CU::GrowingArray<Entity*>& GetSelectedUnits() const;

private:
	CU::Vector3<float> CalcCursorWorldPosition(const Prism::Camera& aCamera);
	void UpdateMouseInteraction(const Prism::Camera& aCamera);
	void SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
		, const CU::Intersection::LineSegment3D& aMouseRay);

	CU::GrowingArray<Entity*> mySelectedUnits;
	GUI::GUIManager* myGUIManager;
	GUI::Cursor* myCursor;

	bool myRenderGUI;
};

inline const CU::GrowingArray<Entity*>& PlayerDirector::GetSelectedUnits() const
{
	return mySelectedUnits;
}