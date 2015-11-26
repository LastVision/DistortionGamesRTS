#pragma once
struct Line3D
{
	Line3D(){};
	Line3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint, const CU::Vector4<float>& aColor);

	CU::Vector4<float> myFirstPoint;
	CU::Vector4<float> mySecondPoint;
	CU::Vector4<float> myColor;
};

