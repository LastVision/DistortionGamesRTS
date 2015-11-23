#include "stdafx.h"

#include <d3dx11effect.h>
#include <D3DX11async.h>
#include <DL_Debug.h>
#include "EffectContainer.h"
#include "EffectListener.h"
#include "Effect.h"
#include "Engine.h"
#include "Texture.h"

Prism::Effect::Effect()
{
	myEffectListeners.Init(512);
	myEffect = nullptr;
}

Prism::Effect::~Effect()
{
	myEffect->Release();
}

bool Prism::Effect::Init(const std::string& aEffectFile)
{
	//if (myEffect != nullptr)
	//{
	//	myEffect->Release();
	//	myEffect = nullptr;
	//}

	if (ReloadShader(aEffectFile) == false)
	{
		return false;
	}

	for (int i = 0; i < myEffectListeners.Size(); ++i)
	{
		myEffectListeners[i]->OnEffectLoad();
	}

	return true;

}

void Prism::Effect::SetScaleVector(const CU::Vector3<float>& aScaleVector)
{
	myScaleVector->SetFloatVector(&aScaleVector.x);
}

void Prism::Effect::SetWorldMatrix(const CU::Matrix44<float>& aWorldMatrix)
{
	myWorldMatrix->SetMatrix(&aWorldMatrix.myMatrix[0]);
}

void Prism::Effect::SetViewMatrix(const CU::Matrix44<float>& aViewMatrix)
{
	myViewMatrix->SetMatrix(&aViewMatrix.myMatrix[0]);
}

void Prism::Effect::SetProjectionMatrix(const CU::Matrix44<float>& aProjectionMatrix)
{
	myProjectionMatrix->SetMatrix(&aProjectionMatrix.myMatrix[0]);
}

void Prism::Effect::SetViewProjectionMatrix(const CU::Matrix44<float>& aMatrix)
{
	myViewProjectionMatrix->SetMatrix(&aMatrix.myMatrix[0]);
}

void Prism::Effect::SetBlendState(ID3D11BlendState* aBlendState, float aBlendFactor[4], const unsigned int aSampleMask)
{
	Engine::GetInstance()->GetContex()->OMSetBlendState(aBlendState, aBlendFactor, aSampleMask);
}

void Prism::Effect::SetTexture(Texture* aTexture)
{
	myTexture->SetResource(aTexture->GetShaderView());
}

void Prism::Effect::SetAmbientHue(CU::Vector4f aVector)
{
	myAmbientHue->SetFloatVector(&aVector.x);
}

void Prism::Effect::SetPosAndScale(const CU::Vector2<float>& aPos
	, const CU::Vector2<float>& aScale)
{
	DL_ASSERT_EXP(mySpritePosAndScale != nullptr
		, "Effect2D: Tried to SetPosAndScale but mySpritePosAndScale is nullptr");

	mySpritePosAndScaleVector.x = aPos.x;
	mySpritePosAndScaleVector.y = aPos.y;

	mySpritePosAndScaleVector.z = aScale.x;
	mySpritePosAndScaleVector.w = aScale.y;

	mySpritePosAndScale->SetFloatVector(&mySpritePosAndScaleVector.x);
}

void Prism::Effect::SetColor(const CU::Vector4<float>& aColor)
{
	mySpriteColor->SetFloatVector(&aColor.x);
}

void Prism::Effect::SetSpriteOrientation(const CU::Matrix44<float>& aOrientation)
{
	mySpriteOrientation->SetMatrix(&aOrientation.myMatrix[0]);
}

void Prism::Effect::UpdateDirectionalLights(
	const CU::StaticArray<Prism::DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData)
{
	if (myDirectionalLight != nullptr)
	{
		myDirectionalLight->SetRawValue(&someDirectionalLightData[0], 0, 
			sizeof(DirectionalLightData) * NUMBER_OF_DIRECTIONAL_LIGHTS);
	}
}

void Prism::Effect::UpdatePointLights(const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData)
{
	if (myPointLight != nullptr)
	{
		myPointLight->SetRawValue(&somePointLightData[0], 0, sizeof(PointLightData) * NUMBER_OF_POINT_LIGHTS);
	}
}

void Prism::Effect::UpdateSpotLights(const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData)
{
	if (mySpotLight != nullptr)
	{
		mySpotLight->SetRawValue(&someSpotLightData[0], 0, sizeof(SpotLightData) * NUMBER_OF_SPOT_LIGHTS);
	}
}

void Prism::Effect::UpdateTime(const float aDeltaTime)
{
	if (myTotalTime != nullptr)
	{
		float newTime = 0.f;
		myTotalTime->GetFloat(&newTime);
		newTime += aDeltaTime;
		myTotalTime->SetFloat(newTime);
	}
}

void Prism::Effect::SetStreakTexture(Texture* aTexture)
{
	myStreakDiffuse->SetResource(aTexture->GetShaderView());
}

void Prism::Effect::SetStreakSizeDelta(float aSizeDelta)
{
	myStreakSizeDelta->SetFloat(aSizeDelta);
}

void Prism::Effect::SetStreakStartAlpha(float aStartAlpha)
{
	myStreakStartAlpha->SetFloat(aStartAlpha);
}

void Prism::Effect::SetStreakAlphaDelta(float anAlphaDelta)
{
	myStreakAlphaDelta->SetFloat(anAlphaDelta);
}


void Prism::Effect::SetPlayerVariable(int someVariable)
{
	if (myPlayerVariable != nullptr)
	{
		myPlayerVariable->SetInt(someVariable);
	}
}

bool Prism::Effect::ReloadShader(const std::string& aFile)
{
	myFileName = aFile;

	HRESULT hr;
	unsigned int shaderFlags = 0;

#if defined (DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
#endif

	ID3D10Blob* compiledShader = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	hr = D3DX11CompileFromFile(myFileName.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader
		, &compilationMsgs, 0);
	if (hr != S_OK)
	{
		if (compilationMsgs != nullptr)
		{
			DL_MESSAGE_BOX((char*)compilationMsgs->GetBufferPointer(), "Effect Error", MB_ICONWARNING);
			myEffect = Engine::GetInstance()->GetEffectContainer()->GetEffect(
				"Data/Resource/Shader/S_effect_debug.fx")->myEffect;
		} 
		else 
		{
			std::string errorMessage = "[Effect]: Could not find the effect " + myFileName;
			DL_ASSERT(errorMessage.c_str());
		}
	}
	if (compilationMsgs != nullptr)
	{
		compilationMsgs->Release();
	}

	if (hr == S_OK)
	{
		hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL
			, Engine::GetInstance()->GetDevice(), &myEffect);

		if (FAILED(hr))
		{
			DL_MESSAGE_BOX("Cant Create Effect", "Effect Error", MB_ICONWARNING);
			return false;
		}

	}

	if (compiledShader != nullptr)
	{
		compiledShader->Release();
	}
	

	myTechnique = myEffect->GetTechniqueByName("Render");
	if (myTechnique->IsValid() == false)
	{
		DL_MESSAGE_BOX("Failed to get Technique", "Effect Error", MB_ICONWARNING);
		return false;
	}

	myScaleVector = myEffect->GetVariableByName("Scale")->AsVector();
	if (myScaleVector->IsValid() == false)
	{
		myScaleVector = nullptr;
	}

	myWorldMatrix = myEffect->GetVariableByName("World")->AsMatrix();
	if (myWorldMatrix->IsValid() == false)
	{
		DL_MESSAGE_BOX("Failed to get WorldMatrix", "Effect Error", MB_ICONWARNING);
		return false;
	}

	myViewMatrix = myEffect->GetVariableByName("View")->AsMatrix();
	if (myViewMatrix->IsValid() == false)
	{
		DL_MESSAGE_BOX("Failed to get ViewMatrix", "Effect Error", MB_ICONWARNING);
		return false;
	}

	myProjectionMatrix = myEffect->GetVariableByName("Projection")->AsMatrix();
	if (myProjectionMatrix->IsValid() == false)
	{
		DL_MESSAGE_BOX("Failed to get ProjectionMatrix", "Effect Error", MB_ICONWARNING);
		return false;
	}

	myViewProjectionMatrix = myEffect->GetVariableByName("ViewProjection")->AsMatrix();
	if (myViewProjectionMatrix->IsValid() == false)
	{
		myViewProjectionMatrix = nullptr;
	}

	myTotalTime = nullptr;
	myTotalTime = myEffect->GetVariableByName("TotalTime")->AsScalar();
	if (myTotalTime->IsValid() == false)
	{
		myTotalTime = nullptr;
	}

	myPlayerVariable = nullptr;
	myPlayerVariable = myEffect->GetVariableByName("PlayerVariable")->AsScalar();
	if (myPlayerVariable->IsValid() == false)
	{
		myPlayerVariable = nullptr;
	}

	myDirectionalLight = myEffect->GetVariableByName("DirectionalLights");
	if (myDirectionalLight->IsValid() == false)
	{
		myDirectionalLight = nullptr;
	}

	myPointLight = myEffect->GetVariableByName("PointLights");
	if (myPointLight->IsValid() == false)
	{
		myPointLight = nullptr;
	}

	mySpotLight = myEffect->GetVariableByName("SpotLights");
	if (mySpotLight->IsValid() == false)
	{
		mySpotLight = nullptr;
	}

	myTexture = myEffect->GetVariableByName("DiffuseTexture")->AsShaderResource();
	if (myTexture->IsValid() == false)
	{
		myTexture = nullptr;
	}

	mySpritePosAndScale = myEffect->GetVariableByName("SpritePositionAndScale")->AsVector();
	if (mySpritePosAndScale->IsValid() == false)
	{
		mySpritePosAndScale = nullptr;
	}

	mySpriteColor = myEffect->GetVariableByName("SpriteColor")->AsVector();
	if (mySpriteColor->IsValid() == false)
	{
		mySpriteColor = nullptr;
	}

	mySpriteOrientation = myEffect->GetVariableByName("SpriteOrientation")->AsMatrix();
	if (mySpriteOrientation->IsValid() == false)
	{
		mySpriteOrientation = nullptr;
	}

	myStreakDiffuse = myEffect->GetVariableByName("DiffuseTexture")->AsShaderResource();
	if (myStreakDiffuse->IsValid() == false)
	{
		myStreakDiffuse = nullptr;
	}

	myStreakSizeDelta = myEffect->GetVariableByName("StreakSizeDelta")->AsScalar();
	if (myStreakSizeDelta->IsValid() == false)
	{
		myStreakSizeDelta = nullptr;
	}

	myStreakStartAlpha = myEffect->GetVariableByName("StreakStartAlpha")->AsScalar();
	if (myStreakStartAlpha->IsValid() == false)
	{
		myStreakStartAlpha = nullptr;
	}

	myStreakAlphaDelta = myEffect->GetVariableByName("StreakAlphaDelta")->AsScalar();
	if (myStreakAlphaDelta->IsValid() == false)
	{
		myStreakAlphaDelta = nullptr;
	}

	myEMPScale = myEffect->GetVariableByName("myEMPScale")->AsScalar();
	if (myEMPScale->IsValid() == false)
	{
		myEMPScale = nullptr;
	}

	myEMPPosition = myEffect->GetVariableByName("myEMPPosition")->AsVector();
	if (myEMPPosition->IsValid() == false)
	{
		myEMPPosition = nullptr;
	}

	myCameraPosition = myEffect->GetVariableByName("cameraPosition")->AsVector();
	if (myCameraPosition->IsValid() == false)
	{
		myCameraPosition = nullptr;
	}

	myAmbientHue = myEffect->GetVariableByName("AmbientHue")->AsVector();
	if (myAmbientHue->IsValid() == false)
	{
		myAmbientHue = nullptr;
	}

	return true;
}

void Prism::Effect::SetCameraPosition(const CU::Vector3<float>& aCameraPos)
{
	if (myCameraPosition != nullptr)
	{
		myCameraPosition->SetFloatVector(static_cast<const float*>(&aCameraPos.x));
	}
}

void Prism::Effect::SetEMPScale(float aScale)
{
	if (myEMPScale != nullptr)
	{
		myEMPScale->SetFloat(aScale);
	}
}

void Prism::Effect::SetEMPPosition(const CU::Vector3<float>& aPosition)
{
	if (myEMPPosition != nullptr)
	{
		myEMPPosition->SetFloatVector(static_cast<const float*>(&aPosition.x));
	}
}
