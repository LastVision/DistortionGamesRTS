#include "stdafx.h"
#include "Line3D.h"


Line3D::Line3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint)
	: myFirstPoint(aFirstPoint)
	, mySecondPoint(aSecondPoint)
{
}