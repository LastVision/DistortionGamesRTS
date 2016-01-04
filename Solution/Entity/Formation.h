#pragma once

namespace Prism
{
	class Terrain;
}

class Formation
{
public:
	Formation(const Prism::Terrain& aTerrain);
	~Formation();

	void ReInit(const CU::GrowingArray<Entity*>& someUnits, const Entity* aTargetUnit = nullptr, const CU::Vector2<float>& aTarget = { -1.f, -1.f });

	bool Update(float aDeltaTime);

private:
	void operator=(Formation&) = delete;
	void Reset();
	void CalcPosition();
	//CU::Vector2<float> myPosition;
	CU::Matrix33<float> myOrientation2D;
	CU::GrowingArray<Entity*> myUnits;
	CU::Vector2<float> myTarget;
	const Entity* myTargetUnit;
	const Prism::Terrain& myTerrain;
	CU::GrowingArray<CU::Vector2<float>> myPath;
	float mySpeed;
};

