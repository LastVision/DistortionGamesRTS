#pragma once
#include <Vector.h>
namespace Prism
{
	enum class VertexType
	{
		POS_COLOR,
		POS_UV,
		POS_COLOR_UV,
		POS_NORM_UV,
		POS_NORM_COLOR,
	};

	struct VertexPosColor
	{
		CU::Vector3<float> myPos;
		CU::Vector4<float> myCol;
	};

	struct VertexPosUV
	{
		CU::Vector3<float> myPos;
		CU::Vector2<float> myUV;
	};

	struct VertexPosColorUV
	{
		CU::Vector3<float> myPos;
		CU::Vector4<float> myCol;
		CU::Vector2<float> myUV;
	};

	struct VertexPosNormUV
	{
		CU::Vector3<float> myPos;
		CU::Vector3<float> myNorm;
		CU::Vector2<float> myUV;
	};

	struct VertexPosNormColor
	{
		CU::Vector3<float> myPos;
		CU::Vector3<float> myNorm;
		CU::Vector4<float> myColor;
	};
}