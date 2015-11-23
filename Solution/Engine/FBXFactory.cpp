#include "stdafx.h"

#include "FBXFactory.h"
#include "FBX/FbxLoader.h"
#include "Matrix44.h"
#include "Model.h"
#include "Surface.h"
#include "IndexBufferWrapper.h"
#include "VertexBufferWrapper.h"
#include "VertexDataWrapper.h"
#include "VertexIndexWrapper.h"
#include <TimerManager.h>


Prism::FBXFactory::FBXFactory()
{
	CU::Matrix44f test;
	test.SetPos(CU::Vector3f(1, 1, 1));
	CU::Matrix44f resultMatrix;

	myLoader = new FBXLoader();
}
	
Prism::FBXFactory::~FBXFactory()
{
	for (unsigned int i = 0; i < myFBXData.size(); ++i)
	{
		delete myFBXData[i];
		myFBXData[i] = nullptr;
	}

	for (auto it = myModels.begin(); it != myModels.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}

	myModels.clear();

	delete myLoader;
}

void Prism::FBXFactory::FillData(ModelData* someData, Model* outData, Effect* aEffect)
{
	VertexIndexWrapper* indexWrapper = new VertexIndexWrapper();
	indexWrapper->myFormat = DXGI_FORMAT_R32_UINT;
	unsigned int* indexData = new unsigned int[someData->myIndexCount];
	memcpy(indexData, someData->myIndicies, someData->myIndexCount*sizeof(unsigned int));
	indexWrapper->myIndexData = (char*)indexData;
	indexWrapper->mySize = someData->myIndexCount*sizeof(unsigned int);
	indexWrapper->myNumberOfIndices = someData->myIndexCount;
	outData->myIndexBaseData = indexWrapper;

	VertexDataWrapper* vertexData = new VertexDataWrapper();
	someData->myVertexBuffer;
	int sizeOfBuffer = someData->myVertexCount*someData->myVertexStride*sizeof(float);
	char* vertexRawData = new char[sizeOfBuffer];
	memcpy(vertexRawData, someData->myVertexBuffer, sizeOfBuffer);
	vertexData->myVertexData = vertexRawData;
	vertexData->myNumberOfVertices = someData->myVertexCount;
	vertexData->mySize = sizeOfBuffer;
	vertexData->myStride = someData->myVertexStride*sizeof(float);

	outData->myVertexBaseData = vertexData;

	for (int i = 0; i < someData->myLayout.Size(); ++i)
	{
		auto currentLayout = someData->myLayout[i];
		D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC();
		desc->SemanticIndex = 0;
		desc->AlignedByteOffset = currentLayout.myOffset;
		desc->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc->InputSlot = 0;
		desc->InstanceDataStepRate = 0;

		if (currentLayout.myType == ModelData::VERTEX_POS)
		{
			desc->SemanticName = "POSITION";
			desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (currentLayout.myType == ModelData::VERTEX_NORMAL)
		{
			desc->SemanticName = "NORMAL";
			desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (currentLayout.myType == ModelData::VERTEX_UV)
		{
			desc->SemanticName = "TEXCOORD";
			desc->Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (currentLayout.myType == ModelData::VERTEX_BINORMAL)
		{
			desc->SemanticName = "BINORMAL";
			desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (currentLayout.myType == ModelData::VERTEX_TANGENT)
		{
			desc->SemanticName = "TANGENT";
			desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (currentLayout.myType == ModelData::VERTEX_SKINWEIGHTS)
		{
			break;
			//desc->SemanticName = "WEIGHTS";
			//desc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		else if (currentLayout.myType == ModelData::VERTEX_BONEID)
		{
			break;
			//desc->SemanticName = "BONES";
			//desc->Format = DXGI_FORMAT_R32G32B32A32_SINT;
		}
		outData->myVertexFormat.Add(desc);
	}
	outData->myIsNULLObject = false;
		
	Surface surface;
	surface.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	surface.SetIndexCount(someData->myIndexCount);
	surface.SetVertexStart(0);
	surface.SetIndexStart(0);
	surface.SetVertexCount(someData->myVertexCount);
	surface.SetEffect(aEffect);
	bool useSRGB = true;
	for (unsigned int i = 0; i < someData->myTextures.size(); ++i)
	{
		auto& currentTexture = someData->myTextures[i];
			
		std::string resourceName;
		if (currentTexture.myType == ALBEDO)
		{
			resourceName = "AlbedoTexture";
		}
		if (currentTexture.myType == NORMAL)
		{
			resourceName = "NormalTexture";
			useSRGB = false;
		}
		if (currentTexture.myType == ROUGHNESS)
		{
			resourceName = "RoughnessTexture";
		}
		if (currentTexture.myType == METALNESS)
		{
			resourceName = "MetalnessTexture";
		}
		if (currentTexture.myType == AMBIENT)
		{
			resourceName = "AOTexture";
		}
		if (currentTexture.myType == FBXTextureType::EMISSIVE)
		{
			resourceName = "EmissiveTexture";
		}

		surface.SetTexture(resourceName, currentTexture.myFileName, useSRGB);
	}
	outData->mySurfaces.Add(new Surface(surface));
	
}

Prism::Model* Prism::FBXFactory::CreateModel(FbxModelData* someModelData, Effect* aEffect)
{
	Model* tempModel = new Model();
	tempModel->SetEffect(aEffect);
	tempModel->SetLodGroup(someModelData->myLodGroup);
		
	if (someModelData->myData)
	{
		FillData(someModelData->myData, tempModel, aEffect);
		tempModel->myOrientation = someModelData->myOrientation;
	}
	for (int i = 0; i < someModelData->myChildren.Size(); ++i)
	{
		auto currentChild = someModelData->myChildren[i];
		tempModel->AddChild(CreateModel(currentChild, aEffect));
	}
	return tempModel;
}

Prism::Model* Prism::FBXFactory::LoadModel(const char* aFilePath, Effect* aEffect)
{
#ifndef DLL_EXPORT
	if (myModels.find(aFilePath) != myModels.end())
	{
		return myModels[aFilePath];
	}
#endif
	DL_DEBUG("Load Model %s", aFilePath);
	CU::TimerManager::GetInstance()->StartTimer("LoadModel");
	FBXData* found = 0;
	for (FBXData* data : myFBXData)
	{ 
		if (data->myPath.compare(aFilePath) == 0)
		{
			found = data;
			break;
		}
	}

	FbxModelData* modelData = nullptr;
#ifndef DLL_EXPORT
	if (found)
	{
		modelData = found->myData;
	}
	else
	{
		FBXData* data = new FBXData();
		FbxModelData* fbxModelData = myLoader->loadModel(aFilePath);
		data->myData = fbxModelData;
		data->myPath = aFilePath;
		myFBXData.push_back(data);
		modelData = data->myData;
	}
#else
	FBXData* data = new FBXData();
	FbxModelData* fbxModelData = myLoader->loadModel(aFilePath);
	data->myData = fbxModelData;
	data->myPath = aFilePath;
	myFBXData.push_back(data);
	modelData = data->myData;
#endif


	Model* returnModel = CreateModel(modelData, aEffect);
	returnModel->myFileName = aFilePath;
	returnModel->Init();

	myModels[aFilePath] = returnModel;

	int elapsed = static_cast<int>(
		CU::TimerManager::GetInstance()->StopTimer("LoadModel").GetMilliseconds());

	if (elapsed > 700)
	{
		RESOURCE_LOG("Model \"%s\" took %d ms to load!!!", aFilePath, elapsed);
	}
	else
	{
		RESOURCE_LOG("Model \"%s\" took %d ms to load", aFilePath, elapsed);
	}
	
	return returnModel;
}

void Prism::FBXFactory::LoadModelForRadiusCalc(const char* aFilePath
	, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut)
{
	FbxModelData* fbxModelData = myLoader->loadModel(aFilePath);

	CreateModelForRadiusCalc(fbxModelData, someVerticesOut);
	delete fbxModelData;
}

void Prism::FBXFactory::CreateModelForRadiusCalc(FbxModelData* someModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
	, const CU::Matrix44<float>& aParentOrientation)
{
	CU::Matrix44<float> orientation = someModelData->myOrientation * aParentOrientation;
	orientation.NormalizeRotationVectors();
	if (someModelData->myData)
	{
		FillDataForRadiusCalc(someModelData->myData, someVerticesOut, orientation);
	}
	for (int i = 0; i < someModelData->myChildren.Size(); ++i)
	{
		auto currentChild = someModelData->myChildren[i];
		CreateModelForRadiusCalc(currentChild, someVerticesOut, orientation);
	}
}

void Prism::FBXFactory::FillDataForRadiusCalc(ModelData* aModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
	, const CU::Matrix44<float>& aOrientation)
{
	int sizeOfBuffer = aModelData->myVertexCount*aModelData->myVertexStride*sizeof(float);
	char* vertexRawData = new char[sizeOfBuffer];
	memcpy(vertexRawData, aModelData->myVertexBuffer, sizeOfBuffer);

	for (int i = 0; i < sizeOfBuffer - 3 * 4; i += aModelData->myVertexStride*sizeof(float))
	{
		CU::Vector3<float> position;
		memcpy(&position, vertexRawData + i, 3 * 4);
		position = position * aOrientation;
		someVerticesOut.Add(position);
	}
	delete vertexRawData;
}