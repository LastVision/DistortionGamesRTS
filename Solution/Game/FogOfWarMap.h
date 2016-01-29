#pragma once
#include "AIMap.h"

class FogOfWarMap :	public AIMap
{
public:
	static FogOfWarMap* GetInstance();
	static void Destroy();

	void Update() override;
	void AddValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition) override;

	void UpdateRenderPlane() override;

	bool IsVisible(const CU::Vector2<float>& aPosition);
	void ToggleFogOfWar();

private:
	FogOfWarMap();
	~FogOfWarMap();

	bool myFogEnabled;
	static FogOfWarMap* myInstance;
};

