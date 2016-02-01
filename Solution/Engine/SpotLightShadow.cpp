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
		myCamera = new Camera(myOrientation);
		//myTexture = new Texture(1920, 1080, eShaderResource::CREATE, eRenderTarget::CREATE, eDepthStencil::DEPTH_ONLY, eTextureFormat::FLOAT);
		myTexture = new Texture();
		//myTexture->InitAsDepthBuffer(myTexture->GetRenderTargetView()); // ?

		myBiasMatrix.myMatrix[0] = 0.5;
		myBiasMatrix.myMatrix[5] = 0.5;
		myBiasMatrix.myMatrix[10] = 0.5;
		myBiasMatrix.SetPos({ 0.5, 0.5, 0.5, 1.f });
	}


	SpotLightShadow::~SpotLightShadow()
	{
		delete myCamera;
		myCamera = nullptr;
	}

	void SpotLightShadow::ClearTexture()
	{
		//float color[4] = { 1.0f, 1.0f, 0.0f, 1.f };

		//Engine::GetInstance()->GetContex()->ClearRenderTargetView(myTexture->myRenderTarget, color);
		//Engine::GetInstance()->GetContex()->ClearDepthStencilView(myTexture->myDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	CU::Matrix44<float> SpotLightShadow::GetMVP() const
	{
		return myOrientation * myCamera->GetProjection();// *myBiasMatrix;
	}

}