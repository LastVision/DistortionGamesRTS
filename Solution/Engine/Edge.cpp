#include "stdafx.h"
#include "Edge.h"
#include "Vertex.h"

//#include "tga2d\engine.h"
//#include "tga2d\drawers\debug_drawer.h"

namespace Prism
{
	namespace Navigation
	{
		Edge::Edge(Vertex* aVertex1, Vertex* aVertex2)
			: myVertex1(aVertex1)
			, myVertex2(aVertex2)
			, myLineSegment(aVertex1->myPosition, aVertex2->myPosition)
			, myTriangle1(nullptr)
			, myTriangle2(nullptr)
		{
			++myVertex1->myEdgeCount;
			++myVertex2->myEdgeCount;

			myPosition3D1.x = myVertex1->myPosition.x;
			myPosition3D1.y = 26.f;
			myPosition3D1.z = myVertex1->myPosition.y;


			myPosition3D2.x = myVertex2->myPosition.x;
			myPosition3D2.y = 26.f;
			myPosition3D2.z = myVertex2->myPosition.y;
		}


		Edge::~Edge()
		{
			// minska count på båda vertex, ta bort om = 0

			--myVertex1->myEdgeCount;
			--myVertex2->myEdgeCount;

			if (myVertex1->myEdgeCount <= 0)
			{
				SAFE_DELETE(myVertex1);
			}
			if (myVertex2->myEdgeCount <= 0)
			{
				SAFE_DELETE(myVertex2);
			}

			// needed for cutting in navMesh..?
			SAFE_DELETE(myTriangle1);
			SAFE_DELETE(myTriangle2);
		}

		void Edge::Render()
		{
			if (myTriangle1 == nullptr || myTriangle2 == nullptr)
			{
				RenderLine3D(myPosition3D1 + CU::Vector3<float>(0, 0.01f, 0), myPosition3D2 + CU::Vector3<float>(0, 0.01f, 0), eColorDebug::YELLOW);
			}
			else
			{
				RenderLine3D(myPosition3D1, myPosition3D2, eColorDebug::RED);
			}
			//DX2D::CDebugDrawer& drawer = DX2D::CEngine::GetInstance()->GetDebugDrawer();

			//DX2D::Vector2f pos1(myVertex1->myPosition.x, myVertex1->myPosition.y);
			//DX2D::Vector2f pos2(myVertex2->myPosition.x, myVertex2->myPosition.y);

			//if (myTriangle1 == nullptr || myTriangle2 == nullptr)
			//{
			//	drawer.DrawLine(pos1, pos2, { 1.f, 0, 0, 1.f });
			//}
			//else
			//{
			//	drawer.DrawLine(pos1, pos2, { 0, 0, 1.f, 1.f });
			//}
		}

		void Edge::Remove(Triangle* aTriangle)
		{
			if (myTriangle1 == aTriangle)
			{
				myTriangle1 = nullptr;
				return;
			}
			else if (myTriangle2 == aTriangle)
			{
				myTriangle2 = nullptr;
				return;
			}

			DL_ASSERT("Triangle not found.");
		}
	}
}