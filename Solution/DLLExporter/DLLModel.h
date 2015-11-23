#pragma once
#include <Vector.h>
#include <Matrix.h>
#include <string>

class Entity;

namespace Prism
{
	class Scene;
}

namespace CU 
{
	class InputWrapper;
}

class DLLModel
{
public:
	DLLModel(CU::InputWrapper& aInput, Prism::Scene& aScene);
	~DLLModel();

	void Update(float aDeltaTime);
	void LoadModel(const char* aModelFile, const char* aEffectFile);

	void SetAutoRotationSpeed(const CU::Vector3f& aRotationSpeed);
	const CU::Vector3f& GetAutoRotationSpeed() const;

	void SetManualRotationAngle(const CU::Vector3f& aRotationAngle);
	const CU::Vector3f& GetManualRotationAngle() const;
private:
	void WatchFiles(const char* aModelFile, const char* aShaderFile);

	void AutoRotate(float aDeltaTime);
	void ManualRotate();

	Entity* myEntity;

	CU::Vector3f myAutoRotationSpeed;
	CU::Vector3f myManualRotationAngle;

	CU::InputWrapper* myInput;

	Prism::Scene* myScene;
};

inline void DLLModel::SetAutoRotationSpeed(const CU::Vector3f& aRotationSpeed)
{
	myAutoRotationSpeed = aRotationSpeed;
}

inline const CU::Vector3f& DLLModel::GetAutoRotationSpeed() const
{
	return myAutoRotationSpeed;
}

inline void DLLModel::SetManualRotationAngle(const CU::Vector3f& aRotationAngle)
{
	myManualRotationAngle = aRotationAngle;
	ManualRotate();
}
inline const CU::Vector3f& DLLModel::GetManualRotationAngle() const
{
	return myManualRotationAngle;
}