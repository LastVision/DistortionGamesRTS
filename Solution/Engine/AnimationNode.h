#pragma once

namespace Easy3D
{
	struct AnimationNodeValue
	{
		float myTime;
		CU::Matrix44<float> myMatrix;
	};

	class AnimationNode
	{
		friend class Animation;
		friend class FBXFactory;
	public:
		AnimationNode(int aNumOfValues);
		~AnimationNode();

		void AddValue(const AnimationNodeValue& aFrame);
		bool HasValues() const;

		CU::Matrix44<float> GetCurrentMatrix(float aTime);
		float GetAnimationLenght();

	private:
		CU::GrowingArray<AnimationNodeValue> myValues;
		float myEndTime;
		std::string myBoneName;
	};
}