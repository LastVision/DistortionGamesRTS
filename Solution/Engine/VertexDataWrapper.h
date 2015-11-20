#pragma once

#include "Vertices.h"

namespace Prism
{
	struct VertexDataWrapper
	{
		int myNumberOfVertices;
		int mySize;
		int myStride;
		VertexType myType;
		char* myVertexData;

		VertexPosColor* GetDataAsVertexPosColor();
	};
}

inline Prism::VertexPosColor* Prism::VertexDataWrapper::GetDataAsVertexPosColor()
{
	return reinterpret_cast<VertexPosColor*>(myVertexData);
}