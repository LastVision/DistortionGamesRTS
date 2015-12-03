#pragma once

#include <Vector.h>

namespace Prism
{
	namespace Navigation
	{
		struct Vertex
		{
			Vertex(const CU::Vector2<float>& aPosition) : myPosition(aPosition), myEdgeCount(0) {}
			CU::Vector2<float> myPosition;
			int myEdgeCount;
		};
	}
}