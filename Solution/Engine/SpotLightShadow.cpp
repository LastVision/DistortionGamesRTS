#include "stdafx.h"
#include "Camera.h"
#include <d3d11.h>
#include "Engine.h"
#include "SpotLightShadow.h"
#include "Texture.h"

namespace Prism
{
	SpotLightShadow::SpotLightShadow()
	{
		//myCamera = new Camera(myOrientation, 25.f, 1024, 1024);
		myOrientation.myMatrix[0] = 0.728f;
		myOrientation.myMatrix[1] = -0.568f;
		myOrientation.myMatrix[2] = 0.383f;
		myOrientation.myMatrix[3] = 0.f;
		myOrientation.myMatrix[4] = 0.f;
		myOrientation.myMatrix[5] = 0.559f;
		myOrientation.myMatrix[6] = 0.829f;
		myOrientation.myMatrix[7] = 0.f;
		myOrientation.myMatrix[8] = -0.686f;
		myOrientation.myMatrix[9] = -0.603f;
		myOrientation.myMatrix[10] = 0.407f;
		myOrientation.myMatrix[11] = 0.f;
		myOrientation.myMatrix[12] = 335.f;
		myOrientation.myMatrix[13] = 75.f;
		myOrientation.myMatrix[14] = 100.f;
		myOrientation.myMatrix[15] = 1.f;

		SetDir(CU::Vector3<float>(0, 0, 1.f));
		SetRange(1000.f);
		SetCone(0.5f);
		Update();

		myCamera = new Camera(myOrientation);
		myTexture = new Texture();
		myTexture->InitAsDepthBuffer();

		myBiasMatrix.myMatrix[0] = 0.5;
		myBiasMatrix.myMatrix[5] = 0.5;
		myBiasMatrix.myMatrix[10] = 0.5;
		myBiasMatrix.SetPos({ 0.5, 0.5, 0.5, 1.f });

	}


	SpotLightShadow::~SpotLightShadow()
	{
		SAFE_DELETE(myCamera);
		SAFE_DELETE(myTexture);
	}

	void SpotLightShadow::ClearTexture()
	{
		float color[4] = { 1.0f, 1.0f, 0.0f, 1.f };

		Engine::GetInstance()->GetContex()->ClearRenderTargetView(myTexture->GetRenderTargetView(), color);
		Engine::GetInstance()->GetContex()->ClearDepthStencilView(myTexture->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	CU::Matrix44<float> SpotLightShadow::GetMVP() const
	{
		return myOrientation * myCamera->GetProjection();// *myBiasMatrix;
	}

}