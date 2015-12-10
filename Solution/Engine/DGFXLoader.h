#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

/*

-------------------------------
	FILEFORMAT / FILELAYOUT
-------------------------------



	------------------------------------------------------
		TYPE	|	INFO
	------------------------------------------------------
		int		|	isNullObject, is 0 if the model is NOT a nullObject
				|	and is 1 if the model IS a nullObject
	------------------------------------------------------
		int		|	isLodgroup, is 0 if the model is NOT a lodGroup
				|	and is 1 if the model IS a lodGroup
	-----------------------------------------------------
		int		|	isAnimated, is 0 if the model is NOT animated
				|	and is 1 if the model IS animated



DGFXVersion - int, the DGFXVersion that the model was exported in
isNullObject - int, either 0 or 1 (false/true)
isLodGroup - int, either 0 or 1 (false/true)
isAnimated - int, either 0 or 1 (false/true)

Model-Data (only if nullObject is 0)
	indexCount, int (number of Indices)
	indexData, array of ints, indexCount-many elements (all IndexData)

	vertexCount, int (number of Vertices)
	vertexStride, int (size of one Vertex)
	vertexData, array of floats, (vertexCount * vertexStride)-many elements (all VertexData)

	layOutCount, int (number of elements in the InputLayout)
		(for each layOutCount)
			layoutOffset, int (byteOffset for the element in InputLayout)
			semanticIndex, int (semanticIndex for the element in InputLayout)
			layoutType, int (vertexType for the element in InputLayout)

	textureCount, int (number of Textures in)
		(for each textureCount)
			textureType, int (texturetype, EMISSIVE, ALBEDO etc)
			texturePathLenght, int (lenght of the texturePathString)
			texturePath, array of char, texturePathLenght-many elements (texturePathString)

	orientation, array of floats, 16 elements (matrix, myOrientation)

Lod-Data (only if lodGroup is 0)
	lodCount, int, (number of lodlevels in the lodGroup)
	lodData, array of Prism::Lod's, lodCount-many elements

	threshHoldCount, int, (number of threshHolds in the lodGroup)
	threshHoldData, array of doubles, threshHoldCount-many elements

Animation-Data
	AnimationBindMatrix, array of floats, 16 elements

	(BuildBoneHierarchyData)
		BoneID, int
		BoneNameLenght, int, lenght of the boneNameStrng
		BoneName, array of char, BonenameLenght-many elements, the boneNameString
		nrOfChildren, int, number of children that the bone has

	NrOfBones, int, Number of bones in the animation
		(for each NrOfBones)
			BoneNameLenght, int, lenght of the boneNameStrng
			BoneName, array of char, BonenameLenght-many elements, the boneNameString
			BoneMatrix, array of floats, 16 elements
			BoneBindMatrix, array of floats, 16 elements

			NrOfFrames, int, number of frames
			NodeValuesData, array of AnimationNodeValue, nrOfFrames-many elements

	AnimationTime, float, the duration of the animation



childCount, int (number of children


*/



namespace Prism
{
	class Animation;
	class Model;
	class ModelAnimated;
	class Effect;
	class HierarchyBone;


	class DGFXLoader
	{
	public:
		DGFXLoader();
		~DGFXLoader();

		Model* LoadModel(const std::string& aFilePath, Effect* aEffect);
		ModelAnimated* LoadAnimatedModel(const std::string& aFilePath, Effect* aEffect);
		Animation* LoadAnimation(const std::string& aFilePath);
		

	private:
		enum eVertexLayout
		{
			VERTEX_POS,
			VERTEX_NORMAL,
			VERTEX_UV,
			VERTEX_BINORMAL,
			VERTEX_TANGENT,
			VERTEX_SKINWEIGHTS,
			VERTEX_BONEID,
		};
		enum eTextureType
		{
			ALBEDO,
			NORMAL,
			ROUGHNESS,
			METALNESS,
			AMBIENT,
			EMISSIVE,
			NR_OF_TEXTURETYPES,
		};

		Model* CreateModel(Effect* aEffect, std::fstream& aStream);
		ModelAnimated* CreateModelAnimated(Effect* aEffect, std::fstream& aStream);

		void LoadModelData(Model* aOutData, Effect* aEffect, std::fstream& aStream);
		void LoadModelAnimatedData(ModelAnimated* aOutData, Effect* aEffect, std::fstream& aStream);

		void LoadLodGroup(Model* aOutData, std::fstream& aStream);

		Animation* LoadAnimation(ModelAnimated* aOutData, std::fstream& aStream);
		void LoadBoneHierarchy(HierarchyBone& aOutBone, std::fstream& aStream);

		std::unordered_map<std::string, Model*> myModels;
		std::unordered_map<std::string, ModelAnimated*> myModelsAnimated;
		std::unordered_map<std::string, Animation*> myAnimations;
	};

}