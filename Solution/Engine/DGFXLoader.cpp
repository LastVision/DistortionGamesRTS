#include "stdafx.h"
#include "DGFXLoader.h"

#include "BaseModel.h"
#include "Model.h"
#include "VertexDataWrapper.h"
#include "VertexIndexWrapper.h"
#include "Surface.h"
#include <TimerManager.h>

namespace Prism
{
	DGFXLoader::DGFXLoader()
	{
	}


	DGFXLoader::~DGFXLoader()
	{
		for (auto it = myModels.begin(); it != myModels.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		myModels.clear();
	}

	Model* DGFXLoader::LoadFromDGFX(const std::string& aFilePath, Effect* aEffect)
	{
		if (myModels.find(aFilePath) != myModels.end())
		{
			return myModels[aFilePath];
		}

		std::string dgfxFile(aFilePath);
		dgfxFile[dgfxFile.length() - 3] = 'd';
		dgfxFile[dgfxFile.length() - 2] = 'g';
		dgfxFile[dgfxFile.length() - 1] = 'f';
		dgfxFile += 'x';
		DL_DEBUG("Load DGFX %s", dgfxFile.c_str());
		CU::TimerManager::GetInstance()->StartTimer("LoadDGFX");


		std::fstream file;
		file.open(dgfxFile.c_str(), std::ios::in | std::ios::binary);

		Model* newModel = LoadModelFromDGFX(aEffect, file);
		file.close();

		newModel->Init();
		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("LoadDGFX").GetMilliseconds());
		RESOURCE_LOG("DGFX \"%s\" took %d ms to load", dgfxFile.c_str(), elapsed);

		myModels[aFilePath] = newModel;

		return newModel;
	}

	Model* DGFXLoader::LoadModelFromDGFX(Effect* aEffect, std::fstream& aStream)
	{
		Model* tempModel = new Model();
		tempModel->SetEffect(aEffect);

		int nullObject;
		aStream.read((char*)&nullObject, sizeof(int));
		if (nullObject == 1)
		{
			tempModel->myIsNULLObject = false;

			LoadModelDataFromDGFX(tempModel, aEffect, aStream);
			CU::Matrix44<float> matrix;
			aStream.read((char*)&tempModel->myOrientation.myMatrix[0], sizeof(float) * 16);
		}

		int childCount;
		aStream.read((char*)&childCount, sizeof(int));
		for (int i = 0; i < childCount; ++i)
		{
			tempModel->AddChild(LoadModelFromDGFX(aEffect, aStream));
		}

		return tempModel;
	}

	void DGFXLoader::LoadModelDataFromDGFX(Model* aOutData, Effect* aEffect, std::fstream& aStream)
	{
		int indexCount;
		aStream.read((char*)&indexCount, sizeof(int)); //Index count

		unsigned int* indexData = new unsigned int[indexCount];
		aStream.read((char*)(indexData), sizeof(int) * indexCount); //All index data

		VertexIndexWrapper* indexWrapper = new VertexIndexWrapper();
		indexWrapper->myFormat = DXGI_FORMAT_R32_UINT;
		indexWrapper->myIndexData = (char*)indexData;
		indexWrapper->mySize = indexCount * sizeof(unsigned int);
		indexWrapper->myNumberOfIndices = indexCount;
		aOutData->myIndexBaseData = indexWrapper;


		int vertexCount;
		aStream.read((char*)&vertexCount, sizeof(int)); //Vertex count
		int stride;
		aStream.read((char*)&stride, sizeof(int)); //Vertex stride

		int sizeOfBuffer = vertexCount * stride * sizeof(float);
		char* vertexRawData = new char[sizeOfBuffer];
		aStream.read(vertexRawData, sizeOfBuffer); //All vertex data

		VertexDataWrapper* vertexData = new VertexDataWrapper();
		vertexData->myVertexData = vertexRawData;
		vertexData->myNumberOfVertices = vertexCount;
		vertexData->mySize = sizeOfBuffer;
		vertexData->myStride = stride*sizeof(float);
		aOutData->myVertexBaseData = vertexData;


		int layoutCount;
		aStream.read((char*)&layoutCount, sizeof(int)); //Inputlayout element count

		for (int i = 0; i < layoutCount; ++i)
		{
			int byteOffset;
			aStream.read((char*)&byteOffset, sizeof(int)); //Inputlayout element count

			D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC();
			desc->SemanticIndex = 0;
			desc->AlignedByteOffset = byteOffset;
			desc->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc->InputSlot = 0;
			desc->InstanceDataStepRate = 0;

			int type;
			aStream.read((char*)&type, sizeof(int)); //semanticName lenght

			if (type == eVertexLayout::VERTEX_POS)
			{
				desc->SemanticName = "POSITION";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_NORMAL)
			{
				desc->SemanticName = "NORMAL";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_UV)
			{
				desc->SemanticName = "TEXCOORD";
				desc->Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_BINORMAL)
			{
				desc->SemanticName = "BINORMAL";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_TANGENT)
			{
				desc->SemanticName = "TANGENT";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_SKINWEIGHTS)
			{
				desc->SemanticName = "WEIGHTS";
				desc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_BONEID)
			{
				desc->SemanticName = "BONES";
				desc->Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}

			aOutData->myVertexFormat.Add(desc);
		}

		Surface surface;
		surface.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		surface.SetIndexCount(indexCount);
		surface.SetVertexStart(0);
		surface.SetIndexStart(0);
		surface.SetVertexCount(vertexCount);
		surface.SetEffect(aEffect);

		int textureCount;
		aStream.read((char*)&textureCount, sizeof(int)); //numberOfTextures
		for (unsigned int i = 0; i < textureCount; ++i)
		{
			int textureType;
			aStream.read((char*)&textureType, sizeof(int)); //resourceName lenght

			std::string resourceName;
			if (textureType == eTextureType::ALBEDO)
			{
				resourceName = "AlbedoTexture";
			}
			if (textureType == eTextureType::NORMAL)
			{
				resourceName = "NormalTexture";
			}
			if (textureType == eTextureType::ROUGHNESS)
			{
				resourceName = "RoughnessTexture";
			}
			if (textureType == eTextureType::METALNESS)
			{
				resourceName = "MetalnessTexture";
			}
			if (textureType == eTextureType::AMBIENT)
			{
				resourceName = "AOTexture";
			}
			if (textureType == eTextureType::EMISSIVE)
			{
				resourceName = "EmissiveTexture";
			}


			int textureLenght;
			aStream.read((char*)&textureLenght, sizeof(int)); //currentTexture.myFileName lenght
			char* texture = new char[textureLenght];
			aStream.read(texture, sizeof(char) * textureLenght); //currentTexture.myFileName
			texture[textureLenght] = '\0';

			surface.SetTexture(resourceName, texture, false);
			delete texture;
		}

		aOutData->mySurfaces.Add(new Surface(surface));
	}
}