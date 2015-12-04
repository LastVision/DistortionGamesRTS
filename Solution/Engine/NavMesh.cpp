#include "stdafx.h"

#include "Edge.h"
#include <Intersection.h>
#include "NavMesh.h"
#include "Triangle.h"
#include "Vertex.h"

namespace Prism
{
	namespace Navigation
	{
		NavMesh::NavMesh()
			: myTriangles(128)
			, myNewTriangles(128)
		{
			Vertex* topLeft = new Vertex(CU::Vector2<float>(0.0f, 0.0f));
			Vertex* topRight = new Vertex(CU::Vector2<float>(255.f, 0.0f));
			Vertex* botLeft = new Vertex(CU::Vector2<float>(0.0f, 255.f));
			Vertex* botRight = new Vertex(CU::Vector2<float>(255.f, 255.f));

			Edge* top = new Edge(topLeft, topRight);
			Edge* left = new Edge(botLeft, topLeft);
			Edge* right = new Edge(topRight, botRight);
			Edge* bot = new Edge(botRight, botLeft);

			Edge* middle = new Edge(topLeft, botRight);

			myTriangles.Add(new Triangle(top, right, middle));
			myTriangles.Add(new Triangle(left, bot, middle));
		}

		NavMesh::~NavMesh()
		{
			myTriangles.DeleteAll();
		}

		void NavMesh::Render()
		{
			for (int i = 0; i < myTriangles.Size(); ++i)
			{
				myTriangles[i]->Render();
			}
			DEBUG_PRINT(myTriangles.Size());
		}

		void NavMesh::Cut(Edge* anEdge)
		{
			CU::GrowingArray<Edge*> edgesToCut(8);
			for (int i = 0; i < myTriangles.Size(); ++i)
			{
				myTriangles[i]->GetEdges(anEdge, edgesToCut);
			}

			for (int i = 0; i < edgesToCut.Size(); ++i)
			{
				if (edgesToCut[i]->myTriangle1 != nullptr && myTriangles.Find(edgesToCut[i]->myTriangle1) >= 0)
				{
					myTriangles.RemoveCyclic(edgesToCut[i]->myTriangle1);
				}
				if (edgesToCut[i]->myTriangle2 != nullptr && myTriangles.Find(edgesToCut[i]->myTriangle2) >= 0)
				{
					myTriangles.RemoveCyclic(edgesToCut[i]->myTriangle2);
				}

				Vertex* intersectionVertex = new Vertex(edgesToCut[i]->myIntersection);
				Edge* newEdge1 = new Edge(edgesToCut[i]->myVertex1, intersectionVertex);
				Edge* newEdge2 = new Edge(intersectionVertex, edgesToCut[i]->myVertex2);


				if (edgesToCut[i]->myTriangle1 != nullptr)
				{
					CutTriangle(edgesToCut[i], edgesToCut[i]->myTriangle1, newEdge1, newEdge2, intersectionVertex);
				}
				if (edgesToCut[i]->myTriangle2 != nullptr)
				{
					CutTriangle(edgesToCut[i], edgesToCut[i]->myTriangle2, newEdge1, newEdge2, intersectionVertex);
				}
			}
			edgesToCut.DeleteAll();
		}

		void NavMesh::CutTriangle(Edge* anEdgeToCut, Triangle* aTriangle, Edge* aNewEdge1, Edge* aNewEdge2, Vertex* anIntersectionVertex)
		{
			Vertex* oppositeVertex = aTriangle->GetOpposite(anEdgeToCut);

			Edge* newEdge3 = new Edge(anIntersectionVertex, oppositeVertex);

			Edge* oldEdge1 = aTriangle->GetOther(anEdgeToCut->myVertex1, anEdgeToCut);
			oldEdge1->Remove(aTriangle);
			myTriangles.Add(new Triangle(oldEdge1, newEdge3, aNewEdge1));
			myNewTriangles.Add(myTriangles.GetLast());

			Edge* oldEdge2 = aTriangle->GetOther(anEdgeToCut->myVertex2, anEdgeToCut);
			oldEdge2->Remove(aTriangle);
			myTriangles.Add(new Triangle(oldEdge2, newEdge3, aNewEdge2));
			myNewTriangles.Add(myTriangles.GetLast());
		}

		void NavMesh::Cut(const CU::GrowingArray<CU::Vector2<float>>& someVertices)
		{
			//only tests AABB for now
			DL_ASSERT_EXP(someVertices.Size() == 4, "can only cut AABB");

			for (int i = 0; i < 4; ++i)
			{
				int first = i;
				int second = i + 1;
				if (second >= someVertices.Size())
				{
					second = 0;
				}
				Edge* edge = new Edge(new Vertex(someVertices[first]), new Vertex(someVertices[second]));

				Cut(edge);
				
				SAFE_DELETE(edge);
			}

			CU::Vector2<float> topLeft(someVertices[0]);

			for (int i = 1; i < someVertices.Size(); ++i)
			{
				topLeft.x = fminf(topLeft.x, someVertices[i].x);
				topLeft.y = fminf(topLeft.y, someVertices[i].y);
			}
			CU::Vector2<float> botRight(someVertices[0]);

			for (int i = 1; i < someVertices.Size(); ++i)
			{
				botRight.x = fmaxf(botRight.x, someVertices[i].x);
				botRight.y = fmaxf(botRight.y, someVertices[i].y);
			}
			
			for (int i = myNewTriangles.Size() - 1; i >= 0; --i)
			{
				if (CU::Intersection::PointVsRect(myNewTriangles[i]->GetCenter(), topLeft, botRight) == true)
				{
					if (myTriangles.Find(myNewTriangles[i]) >= 0)
					{
						myTriangles.DeleteCyclic(myNewTriangles[i]);
					}
				}
			}

			myNewTriangles.RemoveAll();
		}
	}
}