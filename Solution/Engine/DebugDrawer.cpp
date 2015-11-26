#include "stdafx.h"
#include "Engine.h"
#include "DebugDrawer.h"
#include "Line3DRenderer.h"

namespace Prism
{
	DebugDrawer* DebugDrawer::myInstance = nullptr;
	DebugDrawer* DebugDrawer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new DebugDrawer();
		}

		return myInstance;
	}

	void DebugDrawer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	//void DebugDrawer::RenderLine2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderLine3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float> aSecondPoint
		, const CU::Vector4<float>& aColor, const CU::Vector4<float>& aSecondColor)
	{
		my3DLines.Add(Line3D(aFirstPoint, aSecondPoint, aColor, aSecondColor));
	}

	//void DebugDrawer::RenderArrow2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderArrow3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
		, const CU::Vector4<float>& aColor)
	{
		DL_ASSERT("Not Implemented.");
	}

	void DebugDrawer::RenderBox(const CU::Vector3<float>& aPosition, float aSize, const CU::Vector4<float>& aColor
		, bool aWireFrame)
	{
		DL_ASSERT("Not Implemented.");
	}

	void DebugDrawer::RenderSphere(const CU::Vector3<float>& aPosition, float aSize, const CU::Vector4<float>& aColor
		, bool aWireFrame)
	{
		DL_ASSERT("Not Implemented.");
	}

	//void DebugDrawer::RenderText2D(const std::string& aText, const CU::Vector2<float>& aPosition
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderText3D(const std::string& aText, const CU::Vector3<float>& aPosition
		, const CU::Vector4<float>& aColor)
	{
		DL_ASSERT("Not Implemented.");
	}

	void DebugDrawer::Render(const Camera& aCamera)
	{
		myLine3DRenderer->Render(my3DLines, aCamera);


		my3DLines.RemoveAll();
	}

	DebugDrawer::DebugDrawer()
		: my3DLines(1024)
		, myLine3DRenderer(new Line3DRenderer())
	{

	}

	DebugDrawer::~DebugDrawer()
	{
		SAFE_DELETE(myLine3DRenderer);
	}
}