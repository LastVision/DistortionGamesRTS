#include <string>
#include <Camera.h>
#include <DL_Debug.h>
#include "DLLCamera.h"
#include <Entity.h>
#include <EngineEnums.h>
#include <InputComponent.h>
#include <InputWrapper.h>
#include <Scene.h>
#include <SetupInfo.h>

DLLCamera::DLLCamera(CU::InputWrapper& aInput, Prism::SetupInfo& aWindowSetup, Prism::Scene& aScene,
	float aMovementSpeed, float aRotationSpeed, float aZoomSpeed)
	: myInput(aInput)
	, myMovementSpeed(aMovementSpeed)
	, myRotationSpeed(aRotationSpeed)
	, myZoomSpeed(aZoomSpeed)
{
	myEntity = new Entity(eEntityType::PLAYER, aScene, Prism::eOctreeType::DYNAMIC);
	myEntity->AddComponent<InputComponent>()->Init(aInput);

	myCamera = new Prism::Camera(myEntity->myOrientation);
	myCamera->OnResize(aWindowSetup.myScreenWidth, aWindowSetup.myScreenHeight);
	aScene.SetCamera(myCamera);
}


DLLCamera::~DLLCamera()
{
	delete myCamera;
	myCamera = nullptr;

	delete myEntity;
	myEntity = nullptr;
}

void DLLCamera::Zoom(float aDeltaTime, float aMouseSens)
{
	if (HasMouseDeltaXMoved(aMouseSens) == true)
	{
		myCamera->MoveForward(myInput.GetMouseDY() * myZoomSpeed * aDeltaTime);
	}
	if (HasMouseDeltaYMoved(aMouseSens) == true)
	{
		myCamera->MoveForward(myInput.GetMouseDX() * myZoomSpeed * aDeltaTime);
	}
}

void DLLCamera::Pan(float aDeltaTime, float aMouseSens)
{
	if (HasMouseDeltaXMoved(aMouseSens) == true)
	{
		myCamera->MoveRight(myInput.GetMouseDX() * myMovementSpeed * aDeltaTime);
	}
	if (HasMouseDeltaYMoved(aMouseSens) == true)
	{
		myCamera->MoveUp(myInput.GetMouseDY() * myMovementSpeed * aDeltaTime);
	}
}

void DLLCamera::Rotate(float aDeltaTime, float aMouseSens)
{
	CU::Matrix44f rotationAroundObject;
	if (HasMouseDeltaXMoved(aMouseSens) == true)
	{
		rotationAroundObject = myCamera->GetOrientation() * CU::Matrix44f::CreateRotateAroundX(myInput.GetMouseDY()
			* myRotationSpeed * aDeltaTime);
		myCamera->SetOrientation(rotationAroundObject);
	}
	if (HasMouseDeltaYMoved(aMouseSens) == true)
	{
		rotationAroundObject = myCamera->GetOrientation() * CU::Matrix44f::CreateRotateAroundY(myInput.GetMouseDX()
			* myRotationSpeed * aDeltaTime);
		myCamera->SetOrientation(rotationAroundObject);
	}
}

bool DLLCamera::HasMouseDeltaYMoved(float aSens)
{
	return (myInput.GetMouseDY() < aSens || myInput.GetMouseDY() > aSens);
}

bool DLLCamera::HasMouseDeltaXMoved(float aSens)
{
	return (myInput.GetMouseDX() < aSens || myInput.GetMouseDX() > aSens);
}