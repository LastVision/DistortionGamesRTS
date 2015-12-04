#pragma once
#include <GrowingArray.h>
#include <Vector.h>


namespace Prism
{
	namespace Navigation
	{
		class Edge;
		struct Vertex;

		class Triangle
		{
		public:
			friend class PathFinder;
			friend class NavMesh;
			Triangle(Edge* aEdge1, Edge* aEdge2, Edge* aEdge3);
			~Triangle();

			void Render();

			bool Collide(const CU::Vector2<float>& aPosition) const;

			void GetEdges(Edge* aCutEdge, CU::GrowingArray<Edge*>& someEdgesOut) const;
			Vertex* GetOpposite(Edge* anEdge) const;
			Edge* GetOther(Vertex* aSharedVertex, Edge* anEdge) const;
			const CU::Vector2<float>& GetCenter() const;

			float myTotalCost; // needed public for LesserTriangle-template class, can you find a better solution?
		private:
			Triangle* GetOtherTriangle(Edge* anEdge) const;
			Vertex* GetShared(Edge* anEdge1, Edge* anEdge2) const;
			void AddUnique(Edge* anEdge, CU::GrowingArray<Edge*>& someEdgesOut) const;
			void InitEdge(Edge* anEdge);
			void DeleteEdgeIfSolo(Edge* anEdge);
			Edge* GetEdgeWithVertex(Vertex* aVertex, Edge* anEdge1, Edge* anEdge2) const;
			Edge* myEdge1;
			Edge* myEdge2;
			Edge* myEdge3;

			Vertex* myVertex1;
			Vertex* myVertex2;
			Vertex* myVertex3;

			enum class eType
			{
				WALKABLE,
				BLOCKED,
				START,
				PATH,
				EXIT,
			};

			enum class eState
			{
				NONE,
				OPEN,
				CLOSED,
			};

			eType myType;
			eState myState;

			CU::Vector2<float> myCenter;
			float myCurrDist;
			float myCostToGoal;
			bool myChecked;
			Triangle* myPredec;
		};

		inline const CU::Vector2<float>& Triangle::GetCenter() const
		{
			return myCenter;
		}
	}
}