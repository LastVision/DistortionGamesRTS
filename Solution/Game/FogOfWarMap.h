#pragma once
#include "AIMap.h"

class FogOfWarMap :	public AIMap
{
public:
	FogOfWarMap();
	~FogOfWarMap();

	void Update() override;
	void AddValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition) override;

	void UpdateRenderPlane() override;
};

