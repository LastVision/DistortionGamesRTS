#pragma once

namespace Prism
{
	class Ice
	{
	public:
		Ice(Effect* aEffect);
		~Ice();

		void SetTextures();

	private:
		void CreateTextureFromFile(const std::string& aFilePath, ID3D11ShaderResourceView** aResourceToBind);

		Effect* myEffect;
		ID3DX11EffectShaderResourceVariable* mySplatAlbedo;
		ID3DX11EffectShaderResourceVariable* mySplatMetalness;
		ID3DX11EffectShaderResourceVariable* mySplatRoughness;
		ID3DX11EffectShaderResourceVariable* mySplatNormal;
		ID3DX11EffectShaderResourceVariable* mySplatAmbientOcclusion;


		ID3D11ShaderResourceView* myAlbedoTextures[2];
		ID3D11ShaderResourceView* myMetalnessTextures[2];
		ID3D11ShaderResourceView* myRoughnessTextures[2];
		ID3D11ShaderResourceView* myNormalTextures[2];
		ID3D11ShaderResourceView* myAmbientOcclusionTextures[2];
	};

}