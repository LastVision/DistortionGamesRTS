#pragma once

#include <Matrix44.h>

namespace Easy3D
{
	class TransformationNode
	{
	public:
		TransformationNode();
		TransformationNode(const CU::Matrix44<float>& aOrientation);

		virtual const CU::Matrix44<float> GetTransformationForTime(float aTime);
		void SetOrientation(const CU::Matrix44<float>& aOrientation);

	private:
		CU::Matrix44<float> myOrientation;
	};
}