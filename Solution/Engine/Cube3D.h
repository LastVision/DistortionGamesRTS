#pragma once

#include <D3D11.h>

namespace Prism
{
	class Effect;
	class Cube3D
	{
	public:
		Cube3D(const CU::Vector4<float>& aColor);
		~Cube3D();

		void SetPosition(const CU::Vector3<float>& aPosition);
		void SetSizeAndColor(float aSideLength, const CU::Vector4<float>& aColor);

		void Render(const Camera& aCamera);

	private:
		Effect* myEffect;
		VertexIndexWrapper* myIndexBaseData;
		IndexBufferWrapper* myIndexBuffer;
		VertexDataWrapper* myVertexBaseData;
		VertexBufferWrapper* myVertexBuffer;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
		ID3D11InputLayout*	myInputLayout;

		void CreateVertexBuffer(float aSideLength, const CU::Vector4<float>& aColor);
		void InitVertexBuffer();
		void InitIndexBuffer();

		CU::Matrix44<float> myOrientation;
	};

	inline void Cube3D::SetPosition(const CU::Vector3<float>& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

}