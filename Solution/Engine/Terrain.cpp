#include "stdafx.h"

#include "HeightMap.h"
#include "HeightMapFactory.h"
#include "Terrain.h"

#include <D3D11.h>
#include <d3dx11effect.h>
#include "Camera.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Engine.h"
#include "VertexBufferWrapper.h"
#include "IndexBufferWrapper.h"
#include "Surface.h"
#include "TextureContainer.h"

namespace Prism
{
	Terrain::Terrain(const std::string& aHeightMapPath, const std::string& aTexturePath
			, const CU::Vector2<float>& aSize, float aHeight, const CU::Matrix44<float>& aOrientation)
		: myHeightMap(HeightMapFactory::Create(aHeightMapPath.c_str()))
		, mySize(aSize)
		, myHeight(aHeight)
		, myOrientation(aOrientation)
		, myVertexFormat(4)
	{
		myFileName = aTexturePath;

		myEffect = Engine::GetInstance()->GetEffectContainer()->GetEffect("Data/Resource/Shader/S_effect_skybox.fx");
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};



		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "Terrain::InputLayout");
		InitVertexBuffer(sizeof(VertexPosNormUV), D3D11_USAGE_IMMUTABLE, 0);

		InitIndexBuffer();
		InitSurface("DiffuseTexture", myFileName);
		InitBlendState("Terrain::BlendState");

		ZeroMemory(myInitData, sizeof(myInitData));

		CreateVertices();
	}

	Terrain::~Terrain()
	{
		delete myHeightMap;
	}

	void Terrain::Render(const Camera& aCamera)
	{
		CU::Matrix44<float> world;
		world.SetPos(CU::Vector3<float>(0, -110.f, 0));
		myEffect->SetWorldMatrix(world);
		myEffect->SetViewProjectionMatrix(aCamera.GetViewProjection());
		BaseModel::Render();
	}

	void Terrain::CreateVertices()
	{
		CU::GrowingArray<VertexPosNormUV> vertices(myHeightMap->myWidth * myHeightMap->myDepth);
		CU::GrowingArray<int> indices(myHeightMap->myWidth * myHeightMap->myDepth * 6);

		for (int z = 0; z < myHeightMap->myDepth; ++z)
		{
			for (int x = 0; x < myHeightMap->myWidth; ++x)
			{
				VertexPosNormUV vertex;
				vertex.myPos.x = float(x) * mySize.x / float(myHeightMap->myWidth);
				vertex.myPos.y = myHeightMap->myData[z * myHeightMap->myWidth + x] * myHeight / 255.f; 
				vertex.myPos.z = float(z) * mySize.y / float(myHeightMap->myDepth);
				vertex.myUV.x = float(x) / float(myHeightMap->myWidth);
				vertex.myUV.y = float(z) / float(myHeightMap->myDepth);
				vertices.Add(vertex);
			}
		}

		CalcNormals(vertices);

		for (int z = 0; z < myHeightMap->myDepth - 1; ++z)
		{
			for (int x = 0; x < myHeightMap->myWidth - 1; ++x)
			{
				indices.Add(z * myHeightMap->myWidth + x);
				indices.Add(z * myHeightMap->myWidth + x + 1);
				indices.Add((z + 1) * myHeightMap->myWidth + x);

				indices.Add((z + 1) * myHeightMap->myWidth + x);
				indices.Add(z * myHeightMap->myWidth + x + 1);
				indices.Add((z + 1) * myHeightMap->myWidth + x + 1);
			}
		}

		SetupVertexBuffer(vertices.Size(), sizeof(VertexPosNormUV), reinterpret_cast<char*>(&vertices[0])
			, "Terrain::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "Terrain::IndexBuffer");

		mySurfaces[0]->SetVertexCount(vertices.Size());
		mySurfaces[0]->SetIndexCount(indices.Size());
	}

	void Terrain::CalcNormals(CU::GrowingArray<VertexPosNormUV>& someVertices) const
	{
		unsigned int height = myHeightMap->myDepth;
		unsigned int width = myHeightMap->myWidth;
		float yScale = myHeight / 255.f;
		//float xScale = mySize.x / myHeightMap->myDepth;
		float xzScale = mySize.y / myHeightMap->myDepth;


		for (unsigned int y = 0; y<height; ++y)
		{
			for (unsigned int x = 0; x<width; ++x)
			{
				float sx = GetHeight(x<width - 1 ? x + 1 : x, y) - GetHeight(x == 0 ? x - 1 : x, y);
				if (x == 0 || x == width - 1)
					sx *= 2;

				float sy = GetHeight(x, y<height - 1 ? y + 1 : y) - GetHeight(x, y == 0 ? y - 1 : y);
				if (y == 0 || y == height - 1)
					sy *= 2;

				CU::Vector3<float> normal(-sx*yScale, 2 * xzScale, sy*yScale);
				CU::Normalize(normal);

				someVertices[y*width + x].myNorm = normal;
			}
		}
	}

	float Terrain::GetHeight(unsigned int aX, unsigned int aY) const
	{
		return myHeightMap->myData[aY * myHeightMap->myWidth + aX] / 255.f;
	}
}