#pragma once

namespace Easy3D
{
	class AnimationNode;

	class HierarchyBone
	{
		friend class Animation;
		friend class FBXFactory;
	public:
		HierarchyBone();
		~HierarchyBone();

		void Update(const CU::Matrix44<float>& aParentMatrix, float aCurrentTime);

	private:
		CU::Matrix44<float>* myResultMatrix;
		CU::Matrix44<float>* myBoneMatrix;
		AnimationNode* myCurrentAnimation;
		CU::GrowingArray<HierarchyBone> myChildren;
		int myBoneID;
		std::string myBoneName;
	};
}