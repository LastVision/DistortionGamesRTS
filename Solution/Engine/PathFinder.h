#pragma once

#include <GrowingArray.h>
#include <Heap.h>
#include "LesserTriangle.h"
#include "Triangle.h"
#include <Vector.h>

namespace Prism
{
	namespace Navigation
	{
		class NavMesh;

		class PathFinder
		{
		public:
			PathFinder(NavMesh* aNavMesh);
			~PathFinder();
			bool FindPath(const CU::Vector3<float>& aStart, const CU::Vector3<float>& anEnd
				, CU::GrowingArray<Triangle*>& someTrianglesOut);
			bool FindPath(const CU::Vector2<float>& aStart, const CU::Vector2<float>& anEnd
				, CU::GrowingArray<Triangle*>& someTrianglesOut);

			void Render();

		private:
			void operator=(PathFinder&) = delete;
			bool FindStartAndEnd(const CU::Vector2<float>& aStart, const CU::Vector2<float>& anEnd);
			void ClearMesh();
			void AStar();
			void FillPath(CU::GrowingArray<Triangle*>& someTrianglesOut) const;
			float CostEstimate(Triangle* aStart, Triangle* anEnd) const;

			NavMesh* myNavMesh;

			Triangle* myStart;
			Triangle* myEnd;

			float myHeuristicMult;
			float myPathLength;
		};

		inline float PathFinder::CostEstimate(Triangle* aStart, Triangle* anEnd) const
		{
			return myHeuristicMult
				* (fabsf(aStart->myCenter.x - anEnd->myCenter.x)
				+ fabsf(aStart->myCenter.y - anEnd->myCenter.y));
		}
	}
}