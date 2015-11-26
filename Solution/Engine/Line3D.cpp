#include "stdafx.h"
#include "Line3D.h"


Line3D::Line3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
		, const CU::Vector4<float>& aColor)
	: myFirstPoint(aFirstPoint, 1.f)
	, mySecondPoint(aSecondPoint, 1.f)
	, myColor(aColor)
{
}