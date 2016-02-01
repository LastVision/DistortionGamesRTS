#pragma once

#include "SpotLight.h"



namespace Prism
{
	class Camera;
	class Texture;

	class SpotLightShadow : public SpotLight
	{
	public:
		SpotLightShadow();
		~SpotLightShadow();

		Camera* GetCamera() const;
		void ClearTexture();

		Texture* GetTexture() const;
		CU::Matrix44<float> GetMVP() const;

	private:
		Camera* myCamera;
		Texture* myTexture;
		CU::Matrix44<float> myBiasMatrix;

	};


	inline Camera* SpotLightShadow::GetCamera() const
	{
		return myCamera;
	}

	inline Texture* SpotLightShadow::GetTexture() const
	{
		return myTexture;
	}
}