#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

namespace Prism
{
	class Model;
	class Effect;

	class DGFXLoader
	{
	public:
		DGFXLoader();
		~DGFXLoader();

		Model* LoadFromDGFX(const std::string& aFilePath, Effect* aEffect);
		

	private:
		enum eVertexLayout
		{
			VERTEX_POS,
			VERTEX_NORMAL,
			VERTEX_UV,
			VERTEX_BINORMAL,
			VERTEX_TANGENT,
			VERTEX_SKINWEIGHTS,
			VERTEX_BONEID,
		};

		enum eTextureType
		{
			ALBEDO,
			NORMAL,
			ROUGHNESS,
			METALNESS,
			AMBIENT,
			EMISSIVE,
			NR_OF_TEXTURETYPES,
		};

		Model* LoadModelFromDGFX(Effect* aEffect, std::fstream& aStream);
		void LoadModelDataFromDGFX(Model* aOutData, Effect* aEffect, std::fstream& aStream);

		std::unordered_map<std::string, Model*> myModels;
	};

}