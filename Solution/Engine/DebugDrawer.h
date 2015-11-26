#pragma once

#include "Line3D.h"
#include <GrowingArray.h>

enum class eColorDebug
{
	WHITE,
	BLACK,
	RED,
	GREEN,
	BLUE,
	PINK,
	YELLOW,
	NOT_USED
};

namespace Prism
{
	class Camera;
	class Line3DRenderer;
	class Cube3DRenderer;

	class DebugDrawer
	{
	public:
		static DebugDrawer* GetInstance();
		static void Destroy();

		//void RenderLine2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
		//	, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderLine3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float> aSecondPoint
			, eColorDebug aColor = eColorDebug::PINK, eColorDebug aSecondColor = eColorDebug::NOT_USED);
		//void RenderArrow2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
		//	, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderArrow3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
			, eColorDebug aColor = eColorDebug::PINK);
		void RenderBox(const CU::Vector3<float>& aPosition, float aSize = 1.f, eColorDebug aColor = eColorDebug::PINK
			, bool aWireFrame = false);
		void RenderSphere(const CU::Vector3<float>& aPosition, float aSize, eColorDebug aColor = eColorDebug::PINK
			, bool aWireFrame = false);
		//void RenderText2D(const std::string& aText, const CU::Vector2<float>& aPosition
		//	, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderText3D(const std::string& aText, const CU::Vector3<float>& aPosition
			, eColorDebug aColor = eColorDebug::PINK);

		void Render(const Camera& aCamera);

	private:
		static DebugDrawer* myInstance;
		DebugDrawer();
		~DebugDrawer();

		Line3DRenderer* myLine3DRenderer;
		Cube3DRenderer* myCube3DRenderer;

		CU::GrowingArray<Line3D> my3DLines;

		CU::Vector4<float> GetColor(eColorDebug aColor) const;

	};
}