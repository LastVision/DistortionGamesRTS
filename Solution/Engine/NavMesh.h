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
			NavMesh(const std::string& aBinaryPath);
			~NavMesh();

			void Render();
			void Cut(Edge* anEdge);

			const CU::GrowingArray<Triangle*>& GetTriangles() const;
			void Cut(const CU::GrowingArray<CU::Vector2<float>>& someVertices); // needs to be clock wise convex hull

			void Save();

		private:
			void Load(const std::string& aBinaryPath);
			void operator=(NavMesh&) = delete;
			void CreateQuad(Vertex*& aBotLeftVertex, Edge*& aLeftEdge, Edge*& aBotEdge);
			void CutTriangle(Edge* anEdgeToCut, Triangle* aTriangle, Edge* aNewEdge1, Edge* aNewEdge2, Vertex* anIntersectionVertex);
			void UniqueAdd(Edge* anEdge, CU::GrowingArray<Edge*>& someEdgesOut) const;
			void UniqueAdd(Vertex* aVertex, CU::GrowingArray<Vertex*>& someVerticesOut) const;
			void UniqueAddIfExist(Triangle* aTriangle, CU::GrowingArray<Triangle*>& someTrianglesOut) const;
			CU::GrowingArray<Triangle*> myTriangles;
			CU::GrowingArray<Triangle*> myNewTriangles;
			const float myTotalSize;
			const float myCellSize;

		};

		inline const CU::GrowingArray<Triangle*>& NavMesh::GetTriangles() const
		{
			return myTriangles;
		}
	}
}