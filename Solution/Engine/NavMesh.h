#pragma once
#include <GrowingArray.h>
#include <Vector.h>

namespace Prism
{
	namespace Navigation
	{
		class Edge;
		class Triangle;
		struct Vertex;

		class NavMesh
		{
		public:
			NavMesh();
			~NavMesh();

			void Render();
			void Cut(Edge* anEdge);

			const CU::GrowingArray<Triangle*> GetTriangles() const;
			void Cut(const CU::GrowingArray<CU::Vector2<float>>& someVertices); // needs to be clock wise convex hull

		private:
			void CutTriangle(Edge* anEdgeToCut, Triangle* aTriangle, Edge* aNewEdge1, Edge* aNewEdge2, Vertex* anIntersectionVertex);
			CU::GrowingArray<Triangle*> myTriangles;
			CU::GrowingArray<Triangle*> myNewTriangles;
		};

		inline const CU::GrowingArray<Triangle*> NavMesh::GetTriangles() const
		{
			return myTriangles;
		}
	}
}