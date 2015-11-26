#pragma once

#include "Line3D.h"
#include <GrowingArray.h>

#define WHITE_DEBUG CU::Vector4<float>(1.f, 1.f, 1.f, 1.f)
#define BLACK_DEBUG CU::Vector4<float>(0.f, 0.f, 0.f, 1.f)
#define RED_DEBUG CU::Vector4<float>(1.f, 0.f, 0.f, 1.f)
#define GREEN_DEBUG CU::Vector4<float>(0.f, 1.f, 0.f, 1.f)
#define BLUE_DEBUG CU::Vector4<float>(0.f, 0.f, 1.f, 1.f)
#define PINK_DEBUG CU::Vector4<float>(1.f, 0.f, 1.f, 1.f)
#define YELLOW_DEBUG CU::Vector4<float>(1.f, 1.f, 0.f, 1.f)

namespace Prism
{
	class Camera;
	class Line3DRenderer;
	class DebugDrawer
	{
	public:
		static DebugDrawer* GetInstance();
		static void Destroy();

		void RenderLine2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
			, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderLine3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float> aSecondPoint
			, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderArrow2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
			, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderArrow3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
			, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderBox(const CU::Vector3<float>& aPosition, float aSize, const CU::Vector4<float>& aColor = PINK_DEBUG
			, bool aWireFrame = false);
		void RenderSphere(const CU::Vector3<float>& aPosition, float aSize, const CU::Vector4<float>& aColor = PINK_DEBUG
			, bool aWireFrame = false);
		void RenderText2D(const std::string& aText, const CU::Vector2<float>& aPosition
			, const CU::Vector4<float>& aColor = PINK_DEBUG);
		void RenderText3D(const std::string& aText, const CU::Vector3<float>& aPosition
			, const CU::Vector4<float>& aColor = PINK_DEBUG);

		void Render(const Camera& aCamera);

	private:
		static DebugDrawer* myInstance;
		DebugDrawer();
		~DebugDrawer();

		Line3DRenderer* myLine3DRenderer;

		CU::GrowingArray<Line3D> my3DLines;

	};
}