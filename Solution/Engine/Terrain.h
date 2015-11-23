#pragma once

#include "BaseModel.h"
#include <Vector.h>
#include <Matrix.h>
#include <GrowingArray.h>

namespace Prism
{
	class Camera;

	struct HeightMap;
	struct VertexIndexWrapper;
	struct VertexDataWrapper;
	struct VertexPosNormUV;

	class Terrain : public BaseModel
	{
	public:
		Terrain(const std::string& aHeightMapPath, const std::string& aTexturePath
			, const CU::Vector2<float>& aSize, float aHeight, const CU::Matrix44<float>& aOrientation);
		~Terrain();

		void Render(const Camera& aCamera);

	private:
		void operator=(Terrain&) = delete;
		void CreateVertices();
		void CalcNormals(CU::GrowingArray<VertexPosNormUV>& someVertices) const;
		float GetHeight(unsigned int aX, unsigned int aY) const;

		const HeightMap* myHeightMap;
		const CU::Vector2<float> mySize;
		const float myHeight;
		const CU::Matrix44<float> myOrientation;

		CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*> myVertexFormat;
		VertexIndexWrapper* myIndexBaseData;
		VertexDataWrapper* myVertexBaseData;

		int myVertexCount;
	};
}