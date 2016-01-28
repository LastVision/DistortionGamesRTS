#pragma once

namespace Prism
{
	class RenderPlane;
	class Texture;

	class FogOfWarHelper
	{
	public:
		FogOfWarHelper();
		~FogOfWarHelper();
		void Render(Camera& aCamera, RenderPlane* aRenderPlane);

		Texture* GetTexture();

	private:

		Texture* myTexture;
		CU::Matrix44<float> myOrientation;
	};
}