#pragma once

class Entity;

namespace Prism
{
	class Camera;
	class Scene;

	struct SetupInfo;
}

namespace CU
{
	class InputWrapper;
}

class DLLCamera
{
public:
	DLLCamera(CU::InputWrapper& aInput, Prism::SetupInfo& aWindowSetup, Prism::Scene& aScene, 
		float aMovementSpeed, float aRotationSpeed, float aZoomSpeed);
	~DLLCamera();

	void Zoom(float aDeltaTime, float aMouseSens);
	void Pan(float aDeltaTime, float aMouseSens);
	void Rotate(float aDeltaTime, float aMouseSens);

	void SetZoomSpeed(float aValue);
	float GetZoomSpeed() const;

	void SetMovementSpeed(float aValue);
	float GetMovementSpeed() const;

	void SetRotationSpeed(float aValue);
	float GetRotationSpeed() const;
private:
	bool HasMouseDeltaXMoved(float aSens);
	bool HasMouseDeltaYMoved(float aSens);

	CU::InputWrapper& myInput;

	Entity* myEntity;

	Prism::Camera* myCamera;

	float myMovementSpeed;
	float myRotationSpeed;
	float myZoomSpeed;
};

inline void DLLCamera::SetZoomSpeed(float aValue)
{
	myZoomSpeed = aValue;
}
inline float DLLCamera::GetZoomSpeed() const
{
	return myZoomSpeed;
}

inline void DLLCamera::SetMovementSpeed(float aValue)
{
	myMovementSpeed = aValue;
}
inline float DLLCamera::GetMovementSpeed() const
{
	return myMovementSpeed;
}

inline void DLLCamera::SetRotationSpeed(float aValue)
{
	myRotationSpeed = aValue;
}
inline float DLLCamera::GetRotationSpeed() const
{
	return myRotationSpeed;
}