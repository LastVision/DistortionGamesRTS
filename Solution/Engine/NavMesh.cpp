#include "stdafx.h"

#include "Edge.h"
#include <Intersection.h>
#include "NavMesh.h"
#include "Triangle.h"
#include "Vertex.h"
#include <StaticArray.h>

#define QUADS_PER_SIDE 64

namespace Prism
{
	namespace Navigation
	{
		NavMesh::NavMesh()
			: myTriangles(4096)
			, myNewTriangles(1024)
			, myTotalSize(255.f)
			, myCellSize(myTotalSize / QUADS_PER_SIDE)
		{
			Vertex* botLeft = new Vertex(CU::Vector2<float>(0.0f, 0.0f));

			Edge* outRight = nullptr;
			//Edge* outTop = nullptr;
			Edge* bot = nullptr;
			//Edge* left = nullptr;


			CU::StaticArray<Edge*, QUADS_PER_SIDE> topEdges;

			for (int i = 0; i < QUADS_PER_SIDE; ++i)
			{
				topEdges[i] = nullptr;
			}

			for (int j = 0; j < QUADS_PER_SIDE; ++j)
			{
				for (int i = 0; i < QUADS_PER_SIDE; ++i)
				{
					CreateQuad(botLeft, outRight, topEdges[i]);
					//calc new botLeft Vertex
				}

				botLeft = topEdges[0]->myVertex1;
				outRight = nullptr;
			}
		}

		NavMesh::~NavMesh()
		{
			myTriangles.DeleteAll();
		}

		void NavMesh::Render()
		{
			//for (int i = 0; i < myTriangles.Size(); ++i)
			//{
			//	myTriangles[i]->Render();
			//}
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

		void NavMesh::CreateQuad(Vertex*& aBotLeftVertex, Edge*& aLeftEdge, Edge*& aBotEdge)
		{
			Vertex* topRight = new Vertex(CU::Vector2<float>(aBotLeftVertex->myPosition.x + myCellSize, aBotLeftVertex->myPosition.y + myCellSize));

			Vertex* botRight;
			Edge* bot;
			if (aBotEdge != nullptr)
			{
				botRight = aBotEdge->myVertex2; // ensure this is correct
				bot = aBotEdge;
			}
			else
			{
				botRight = new Vertex(CU::Vector2<float>(aBotLeftVertex->myPosition.x + myCellSize, aBotLeftVertex->myPosition.y));
				bot = new Edge(aBotLeftVertex, botRight);
			}

			Vertex* topLeft;
			Edge* left;
			if (aLeftEdge != nullptr)
			{
				topLeft = aLeftEdge->myVertex2; // ensure this is correct
				left = aLeftEdge;
			}
			else
			{
				topLeft = new Vertex(CU::Vector2<float>(aBotLeftVertex->myPosition.x, aBotLeftVertex->myPosition.y + myCellSize));
				left = new Edge(aBotLeftVertex, topLeft);
			}
			
			Edge* right = new Edge(botRight, topRight);
			Edge* top = new Edge(topLeft, topRight);
			Edge* middle = new Edge(aBotLeftVertex, topRight);

			myTriangles.Add(new Triangle(bot, right, middle));
			myTriangles.Add(new Triangle(left, top, middle));

			aBotLeftVertex = botRight;
			aBotEdge = top;
			aLeftEdge= right;
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