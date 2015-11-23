#include "stdafx.h"

#include <CommonHelper.h>
#include <d3dx11effect.h>
#include "Engine.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "FileWatcher.h"
#include "Texture.h"
#include "TextureContainer.h"

Prism::EffectContainer::~EffectContainer()
{
	for (auto it = myEffects.begin(); it != myEffects.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}

	myEffects.clear();
}

Prism::Effect* Prism::EffectContainer::GetEffect(const std::string& aFilePath)
{
	auto it = myEffects.find(aFilePath);

	if (it == myEffects.end())
	{
		LoadEffect(aFilePath);
	}

	return myEffects[aFilePath];
}

void Prism::EffectContainer::LoadEffect(const std::string& aFilePath)
{
	VerifyShader(aFilePath);

	Effect* newEffect = new Effect();
	
	if (newEffect->Init(aFilePath) == false)
	{
		std::stringstream ss;
		ss << "Failed to Init Effect: " << aFilePath.c_str() << std::endl;
		DL_MESSAGE_BOX(ss.str().c_str(), "EffectContainer::LoadEffect", MB_ICONWARNING);
		return;
	}

	if (myCubeMap != "")
	{
		Texture* tex = Engine::GetInstance()->GetTextureContainer()->GetTexture(myCubeMap);
		ID3DX11EffectShaderResourceVariable* shaderVar = newEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

		if (shaderVar->IsValid())
		{
			shaderVar->SetResource(tex->GetShaderView());
		}
	}

	DL_ASSERT_EXP(newEffect != nullptr, "newEffect is nullpter in LoadEffect, HOW?");

	myEffects[aFilePath] = newEffect;

	WATCH_FILE(aFilePath, Prism::EffectContainer::ReloadEffect);
}

void Prism::EffectContainer::ReloadEffect(const std::string& aFilePath)
{
	if (myEffects.find(aFilePath) == myEffects.end())
	{
		return;
	}

	myEffects[aFilePath]->Init(aFilePath);


	Texture* tex = Engine::GetInstance()->GetTextureContainer()->GetTexture(myCubeMap);
	ID3DX11EffectShaderResourceVariable* shaderVar = myEffects[aFilePath]->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

	if (shaderVar->IsValid())
	{
		shaderVar->SetResource(tex->GetShaderView());
	}
}

void Prism::EffectContainer::VerifyShader(const std::string& aFilePath)
{
	//DL_ASSERT_EXP(	CU::Concatenate("Found invalid Shader: %s", aFilePath.c_str()));
}

void Prism::EffectContainer::Update(const float aDeltaTime)
{
	for (auto it = myEffects.begin(); it != myEffects.end(); ++it)
	{
		it->second->UpdateTime(aDeltaTime);
	}
}

void Prism::EffectContainer::SetCubeMap(const std::string& aFilePath)
{
	if (aFilePath != myCubeMap)
	{
		myCubeMap = aFilePath;
		for (auto it = myEffects.begin(); it != myEffects.end(); ++it)
		{
			Texture* tex = Engine::GetInstance()->GetTextureContainer()->GetTexture(myCubeMap);
			ID3DX11EffectShaderResourceVariable* shaderVar = it->second->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

			if (shaderVar->IsValid())
			{
				shaderVar->SetResource(tex->GetShaderView());
			}
		}
	}	
}