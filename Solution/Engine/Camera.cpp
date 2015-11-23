#include "stdafx.h"
#include "Camera.h"
#include "FileWatcher.h"
#include "Frustum.h"
#include <xnamath.h>
#include <XMLReader.h>

namespace Prism
{
	Camera::Camera(CU::Matrix44f& aPlayerMatrix)
		: myOrientation(aPlayerMatrix)
		, myNear(0.1f)
		, myFar(1000.f)
		, myShakeCamera(false)
		, myCurrentShake(0.f)
		, myMaxShake(0.f)
		, myRotateRate(0.f)
		, myMaxShakeTime(0.f)
		, myCurrentShakeTime(0.f)
	{
		myFrustum = new Frustum(aPlayerMatrix, myNear, myFar);
		WATCH_FILE("Data/Setting/SET_camera.xml", Camera::ReadXML);
		ReadXML("Data/Setting/SET_camera.xml");
	}


	Camera::~Camera()
	{
		delete myFrustum;
	}

	void Camera::ReadXML(const std::string& aFileName)
	{
		Sleep(10);
		XMLReader reader;
		reader.OpenDocument(aFileName);
		tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("camera");
		reader.ForceReadAttribute(levelElement, "fov", myFOV);
		reader.ForceReadAttribute(levelElement, "nearplane", myNear);
		reader.ForceReadAttribute(levelElement, "farplane", myFar);
		myFOV *= 3.14159f / 180.f;
		OnResize(Engine::GetInstance()->GetWindowSize().x, Engine::GetInstance()->GetWindowSize().y);
		reader.CloseDocument();
	}

	void Camera::OnResize(const int aWidth, const int aHeight)
	{
		myProjectionMatrix = CU::Matrix44<float>::CreateProjectionMatrixLH(myNear, myFar, static_cast<float>(aHeight) / static_cast<float>(aWidth), myFOV);
		myFrustum->OnResize(myNear, myFar);
	}

	const CU::Matrix44<float>& Camera::GetOrientation() const
	{
		return myOrientation;
	}

	const CU::Matrix44<float>& Camera::GetProjection() const
	{
		return myProjectionMatrix;
	}

	const CU::Matrix44<float>& Camera::GetViewProjection() const
	{
		return myViewProjectionMatrix;
	}

	void Camera::Update(float aDeltaTime)
	{
		if (myShakeCamera == true)
		{
			if (myCurrentShake < -myMaxShake || myCurrentShake > myMaxShake)
			{
				myRotateRate = -myRotateRate;
			}
			myCurrentShake += myRotateRate * aDeltaTime;

			myCurrentShakeTime += aDeltaTime;
			if (myCurrentShakeTime >= myMaxShakeTime)
			{
				myShakeCamera = false;
				myCurrentShakeTime = 0.f;
				myCurrentShake = 0.f;
			}
			RotateZ(myCurrentShake * aDeltaTime);
		}

		myViewProjectionMatrix = CU::InverseSimple(myOrientation) * myProjectionMatrix;

		myFrustum->Update();
	}
	void Camera::SetOrientation(const CU::Matrix44<float>& aOrientation)
	{
		myOrientation = aOrientation;
	}

	void Camera::SetPosition(const CU::Vector3<float>& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	void Camera::RotateX(const float aDegrees)
	{
		CU::Vector3<float> position = myOrientation.GetPos();
		myOrientation.SetPos({ 0.f, 0.f, 0.f, 1.f });
		myOrientation = CU::Matrix44<float>::CreateRotateAroundX(aDegrees * XM_PI / 180.f) * myOrientation;
		myOrientation.SetPos(position);
	}

	void Camera::RotateY(const float aDegrees)
	{
		CU::Vector3<float> position = myOrientation.GetPos();
		myOrientation.SetPos({ 0.f, 0.f, 0.f, 1.f });
		myOrientation = CU::Matrix44<float>::CreateRotateAroundY(aDegrees * XM_PI / 180.f) * myOrientation;
		myOrientation.SetPos(position);
	}

	void Camera::RotateZ(const float aDegrees)
	{
		CU::Vector3<float> position = myOrientation.GetPos();
		myOrientation.SetPos({ 0.f, 0.f, 0.0f, 1.f });
		myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(aDegrees * XM_PI / 180.f) * myOrientation;
		myOrientation.SetPos(position);
	}

	void Camera::MoveForward(const float aDistance)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetForward() * aDistance);
	}

	void Camera::MoveRight(const float aDistance)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetRight() * aDistance);
	}

	void Camera::MoveUp(const float aDistance)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetUp() * aDistance);
	}

	void Camera::ShakeCamera(float aRotationRate, float aMaxRotation, float aTime)
	{
		if (myShakeCamera == false)
		{
			myRotateRate = aRotationRate;
			myMaxShake = aMaxRotation;
			myMaxShakeTime = aTime;
			myCurrentShake = 0.f;
			myCurrentShakeTime = 0.f;
			myShakeCamera = true;
		}
	}
}