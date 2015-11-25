#pragma once

#include "BaseModel.h"
#include "EffectListener.h"
#include <GrowingArray.h>
#include <Matrix.h>
#include "Vertices.h"
#include "LodStructs.h"


namespace Prism
{
	class Animation;
	class Effect;
	class Surface;
	class TransformationNode;

	struct IndexBufferWrapper;
	struct MeshData;
	struct VertexBufferWrapper;
	struct VertexDataWrapper;
	struct VertexIndexWrapper;


	class ModelAnimated : public BaseModel
	{
		friend class FBXFactory;
		friend class Instance;
	public:
		ModelAnimated();
		~ModelAnimated();

		void Init();
		void InitCube(float aWidth = 1.f, float aHeight = 1.f, float aDepth = 1.f
			, CU::Vector4f aColour = { 0.7f, 0.7f, 0.7f, 1.f });

		void AddChild(ModelAnimated* aChild);
		void SetLodGroup(LodGroup* aLodGroup);

		void SetEffect(Effect* aEffect);

		void Render(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aCameraPosition);

	private:
		bool myIsNULLObject;

		CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*> myVertexFormat;
		VertexIndexWrapper* myIndexBaseData;
		VertexDataWrapper* myVertexBaseData;

		CU::GrowingArray<ModelAnimated*> myChildren;
		CU::GrowingArray<TransformationNode*> myChildTransforms;
		TransformationNode* myTransformation;
		Animation* myAnimation;
		CU::Matrix44f myOrientation;

		int myVertexCount;
		ModelAnimated* myParent;
		bool myInited;
		bool myIsLodGroup;
		LodGroup* myLodGroup;
	};
}