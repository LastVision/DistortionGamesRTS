#pragma once
struct Line3D
{
	Line3D(){};
	Line3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint);

	CU::Vector3<float> myFirstPoint;
	CU::Vector3<float> mySecondPoint;
};

