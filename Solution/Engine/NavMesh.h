#pragma once
#include <GrowingArray.h>
#include <Vector.h>

namespace Prism
{
	namespace Navigation
	{
		class NavMesh
		{
		public:
			NavMesh();
			~NavMesh();
			void Cut(const CU::GrowingArray<CU::Vector2<float>>& someVertices); // needs to be clock wise convex hull
		};
	}
}