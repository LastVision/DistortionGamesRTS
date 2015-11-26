#pragma once

#include <D3D11.h>

namespace Prism
{
	class Effect;

	struct VertexBufferWrapper;
	struct IndexBufferWrapper;
	class Line3DRenderer
	{
	public:
		Line3DRenderer();
		~Line3DRenderer();

	private:
		VertexBufferWrapper* myVertexBuffer;
		IndexBufferWrapper* myIndexBuffer;
		D3D11_BUFFER_DESC* myVertexBufferDesc;
		D3D11_BUFFER_DESC* myIndexBufferDesc;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;

		Effect* myEffect;

		void InitVertexBuffer();
		void InitIndexBuffer();
	};

}