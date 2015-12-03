#pragma once

#include <LineSegment2D.h>
#include <Vector.h>

namespace Prism
{
	namespace Navigation
	{
		class Triangle;
		struct Vertex;
		class NavMesh;

		class Edge
		{
		public:
			friend class NavMesh;
			friend class Triangle;
			Edge(Vertex* aVertex1, Vertex* aVertex2);
			~Edge();

			void Render();
			void Remove(Triangle* aTriangle);

			const CU::Intersection::LineSegment2D myLineSegment;

		private:
			void operator=(Edge&) = delete;
			Vertex* myVertex1;
			Vertex* myVertex2;
			Triangle* myTriangle1;
			Triangle* myTriangle2;
			CU::Vector2<float> myIntersection;

			CU::Vector3<float> myPosition3D1;
			CU::Vector3<float> myPosition3D2;
		};
	}
}