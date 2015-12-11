#include "stdafx.h"

#include "Animation.h"
#include "AnimationNode.h"
#include "BaseModel.h"
#include <CommonHelper.h>
#include "DGFXLoader.h"
#include "Model.h"
#include "ModelAnimated.h"
#include "HierarchyBone.h"
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

		for (auto it = myModelsAnimated.begin(); it != myModelsAnimated.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		myModelsAnimated.clear();
	}

	Model* DGFXLoader::LoadModel(const std::string& aFilePath, Effect* aEffect)
	{
		if (myModels.find(aFilePath) != myModels.end())
		{
			return myModels[aFilePath];
		}

		std::string dgfxFile = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

		CU::TimerManager::GetInstance()->StartTimer("LoadDGFX");


		std::fstream file;
		file.open(dgfxFile.c_str(), std::ios::in | std::ios::binary);
		DL_ASSERT_EXP(file.fail() == false, CU::Concatenate("Failed to open %s, did you forget to run the tool?", dgfxFile.c_str()));


		Model* newModel = CreateModel(aEffect, file);
		file.close();

		newModel->Init();
		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("LoadDGFX").GetMilliseconds());
		RESOURCE_LOG("DGFX-Model \"%s\" took %d ms to load", dgfxFile.c_str(), elapsed);

		myModels[aFilePath] = newModel;

		return newModel;
	}

	ModelAnimated* DGFXLoader::LoadAnimatedModel(const std::string& aFilePath, Effect* aEffect)
	{
		if (myModelsAnimated.find(aFilePath) != myModelsAnimated.end())
		{
			return myModelsAnimated[aFilePath];
		}

		std::string dgfxFile = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

		CU::TimerManager::GetInstance()->StartTimer("LoadDGFXAnimated");


		std::fstream file;
		file.open(dgfxFile.c_str(), std::ios::in | std::ios::binary);
		DL_ASSERT_EXP(file.fail() == false, CU::Concatenate("Failed to open %s, did you forget to run the tool?", dgfxFile.c_str()));

		ModelAnimated* newModel = CreateModelAnimated(aEffect, file);

		file.close();

		newModel->Init();

		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("LoadDGFXAnimated").GetMilliseconds());
		RESOURCE_LOG("Animated DGFX-Model \"%s\" took %d ms to load", dgfxFile.c_str(), elapsed);

		myModelsAnimated[aFilePath] = newModel;

		return newModel;
	}

	Animation* DGFXLoader::LoadAnimation(const std::string& aFilePath)
	{
		if (myAnimations.find(aFilePath) != myAnimations.end())
		{
			return myAnimations[aFilePath];
		}

		std::string dgfxFile = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

		CU::TimerManager::GetInstance()->StartTimer("LoadAnimationDGFX");

		std::fstream stream;
		stream.open(dgfxFile.c_str(), std::ios::in | std::ios::binary);
		DL_ASSERT_EXP(stream.fail() == false, CU::Concatenate("Failed to open %s, did you forget to run the tool?", dgfxFile.c_str()));

		int fileVersion;
		stream.read((char*)&fileVersion, sizeof(int));
		DL_ASSERT_EXP(fileVersion == DGFX_VERSION, "Found a old DGFX-Animation, try running the Converter again");

		int isNullObject;
		stream.read((char*)&isNullObject, sizeof(int));

		int isLodGroup;
		stream.read((char*)&isLodGroup, sizeof(int));
		DL_ASSERT_EXP(isLodGroup == 0, "AnimatedModel cant be LOD'ed, not supported yet");

		int isAnimated;
		stream.read((char*)&isAnimated, sizeof(int));


		Animation* animation = LoadAnimation(nullptr, stream);

		stream.close();

		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("LoadAnimationDGFX").GetMilliseconds());
		RESOURCE_LOG("DGFX-Animation \"%s\" took %d ms to load", dgfxFile.c_str(), elapsed);

		myAnimations[aFilePath] = animation;

		return animation;
	}

	Model* DGFXLoader::CreateModel(Effect* aEffect, std::fstream& aStream)
	{
		Model* tempModel = new Model();
		tempModel->SetEffect(aEffect);

		int fileVersion;
		aStream.read((char*)&fileVersion, sizeof(int));
		DL_ASSERT_EXP(fileVersion == DGFX_VERSION, "Found a old DGFX-Model, try running the Converter again");

		int isNullObject;
		aStream.read((char*)&isNullObject, sizeof(int));

		int isLodGroup;
		aStream.read((char*)&isLodGroup, sizeof(int));

		int isAnimated;
		aStream.read((char*)&isAnimated, sizeof(int));

		if (isNullObject == 0)
		{
			tempModel->myIsNULLObject = false;

			LoadModelData(tempModel, aEffect, aStream);
			CU::Matrix44<float> matrix;
			aStream.read((char*)&tempModel->myOrientation.myMatrix[0], sizeof(float) * 16);
		}

		if (isLodGroup == 1)
		{
			LoadLodGroup(tempModel, aStream);
		}

		int childCount;
		aStream.read((char*)&childCount, sizeof(int));
		for (int i = 0; i < childCount; ++i)
		{
			tempModel->AddChild(CreateModel(aEffect, aStream));
		}

		return tempModel;
	}

	ModelAnimated* DGFXLoader::CreateModelAnimated(Effect* aEffect, std::fstream& aStream)
	{
		ModelAnimated* tempModel = new ModelAnimated();
		tempModel->SetEffect(aEffect);

		int fileVersion;
		aStream.read((char*)&fileVersion, sizeof(int));
		DL_ASSERT_EXP(fileVersion == DGFX_VERSION, "Found a old Animated DGFX-Model, try running the Converter again");

		int isNullObject;
		aStream.read((char*)&isNullObject, sizeof(int));

		int isLodGroup;
		aStream.read((char*)&isLodGroup, sizeof(int));
		DL_ASSERT_EXP(isLodGroup == 0, "AnimatedModel cant be LOD'ed, not supported yet");

		int isAnimated;
		aStream.read((char*)&isAnimated, sizeof(int));

		if (isNullObject == 0)
		{
			tempModel->myIsNULLObject = false;
			LoadModelAnimatedData(tempModel, aEffect, aStream);
			CU::Matrix44<float> matrix;
			aStream.read((char*)&tempModel->myOrientation.myMatrix[0], sizeof(float) * 16);
		}

		if (isAnimated == 1)
		{
			LoadAnimation(tempModel, aStream);
		}

		int childCount;
		aStream.read((char*)&childCount, sizeof(int));
		for (int i = 0; i < childCount; ++i)
		{
			tempModel->AddChild(CreateModelAnimated(aEffect, aStream));
		}

		return tempModel;
	}

	void DGFXLoader::LoadModelData(Model* aOutData, Effect* aEffect, std::fstream& aStream)
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

			int semanticIndex;
			aStream.read((char*)&semanticIndex, sizeof(int)); //Inputlayout semantic index


			D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC();
			desc->SemanticIndex = semanticIndex;
			desc->AlignedByteOffset = byteOffset;
			desc->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc->InputSlot = 0;
			desc->InstanceDataStepRate = 0;

			int type;
			aStream.read((char*)&type, sizeof(int)); //element type

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
			else if (type == eVertexLayout::VERTEX_COLOR)
			{
				desc->SemanticName = "COLOR";
				desc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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
		for (int i = 0; i < textureCount; ++i)
		{
			int textureType;
			aStream.read((char*)&textureType, sizeof(int)); //textureType

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

	void DGFXLoader::LoadModelAnimatedData(ModelAnimated* aOutData, Effect* aEffect, std::fstream& aStream)
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

			int semanticIndex;
			aStream.read((char*)&semanticIndex, sizeof(int)); //Inputlayout semantic index

			D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC();
			desc->SemanticIndex = semanticIndex;
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
		for (int i = 0; i < textureCount; ++i)
		{
			int textureType;
			aStream.read((char*)&textureType, sizeof(int)); //textureType

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

	void DGFXLoader::LoadLodGroup(Model* aOutData, std::fstream& aStream)
	{
		Prism::LodGroup* lodGroup = new Prism::LodGroup();

		int lodCount;
		aStream.read((char*)&lodCount, sizeof(int));

		lodGroup->myLods.Reserve(lodCount);
		aStream.read((char*)&lodGroup->myLods[0], sizeof(Prism::Lod) * lodCount);


		int threshHoldCount;
		aStream.read((char*)&threshHoldCount, sizeof(int));

		lodGroup->myThreshHolds.Reserve(threshHoldCount);
		aStream.read((char*)&lodGroup->myThreshHolds[0], sizeof(double) * threshHoldCount);

		aOutData->SetLodGroup(lodGroup);
	}

	Animation* DGFXLoader::LoadAnimation(ModelAnimated* aOutData, std::fstream& aStream)
	{
		CU::Matrix44<float> bindMatrix;
		aStream.read((char*)&bindMatrix.myMatrix[0], sizeof(float) * 16);

		HierarchyBone rootBone;
		LoadBoneHierarchy(rootBone, aStream);

		Animation* newAnimation = new Animation();


		int nrOfbones;
		aStream.read((char*)&nrOfbones, sizeof(int));

		for (int i = 0; i < nrOfbones; ++i)
		{
			int boneNameLenght;
			aStream.read((char*)&boneNameLenght, sizeof(int));
			char* boneName = new char[boneNameLenght];
			boneName[boneNameLenght] = '\0';
			aStream.read(boneName, sizeof(char) * boneNameLenght);

			CU::Matrix44<float> boneMatrix;
			aStream.read((char*)&boneMatrix.myMatrix[0], sizeof(float) * 16);

			CU::Matrix44<float> boneBindMatrix;
			aStream.read((char*)&boneBindMatrix.myMatrix[0], sizeof(float) * 16);


			int nrOfFrames;
			aStream.read((char*)&nrOfFrames, sizeof(int));

			
			AnimationNode* newNode = new AnimationNode(nrOfFrames);
			newNode->myValues.Reserve(nrOfFrames);

			aStream.read((char*)&newNode->myValues[0], sizeof(AnimationNodeValue) * nrOfFrames);
			newNode->CalculateEndTime();

			newNode->myBoneName = boneName;
			newAnimation->AddAnimation(newNode);

			newAnimation->SetBoneMatrix(boneMatrix, i);
			newAnimation->SetBoneBindPose(boneBindMatrix, i);
			newAnimation->AddBoneName(boneName);

			newAnimation->SetBindMatrix(bindMatrix);
			delete[] boneName;
		}

		newAnimation->SetHierarchy(rootBone);

		float animationLenght;
		aStream.read((char*)&animationLenght, sizeof(float));

		newAnimation->SetAnimationLenght(animationLenght);

		if (aOutData != nullptr)
		{
			aOutData->myAnimation = newAnimation;
		}

		return newAnimation;
	}

	void DGFXLoader::LoadBoneHierarchy(HierarchyBone& aOutBone, std::fstream& aStream)
	{
		aStream.read((char*)&aOutBone.myBoneID, sizeof(int));
		int boneNameLenght;
		aStream.read((char*)&boneNameLenght, sizeof(int));
		char* boneName = new char[boneNameLenght];
		boneName[boneNameLenght] = '\0';
		aStream.read(boneName, sizeof(char) * boneNameLenght);
		aOutBone.myBoneName = boneName;
		
		int nrOfChildren;
		aStream.read((char*)&nrOfChildren, sizeof(int));

		if (nrOfChildren > 0)
		{
			aOutBone.myChildren.Init(nrOfChildren);
			for (int i = 0; i < nrOfChildren; ++i)
			{
				HierarchyBone child;
				LoadBoneHierarchy(child, aStream);
				aOutBone.myChildren.Add(child);
			}
		}

		delete[] boneName;
	}
}